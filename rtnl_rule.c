/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * Copyright (c) 2017 JingPiao Chen <chenjingpiao@gmail.com>
 * Copyright (c) 2016-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include "netlink_route.h"
#include "nlattr.h"
#include "print_fields.h"

#include "netlink.h"
#include "types/fib_rules.h"

#include "xlat/fib_rule_actions.h"
#include "xlat/fib_rule_flags.h"
#include "xlat/rtnl_rule_attrs.h"

static bool
decode_rule_addr(struct tcb *const tcp,
		 const kernel_ulong_t addr,
		 const unsigned int len,
		 const void *const opaque_data)
{
	const struct_fib_rule_hdr *const msg = opaque_data;

	decode_inet_addr(tcp, addr, len, msg->family, NULL);

	return true;
}

static bool
decode_fib_rule_uid_range(struct tcb *const tcp,
			  const kernel_ulong_t addr,
			  const unsigned int len,
			  const void *const opaque_data)
{
	struct_fib_rule_uid_range range;

	if (len < sizeof(range))
		return false;
	else if (!umove_or_printaddr(tcp, addr, &range)) {
		PRINT_FIELD_U("{", range, start);
		PRINT_FIELD_U(", ", range, end);
		tprints("}");
	}

	return true;
}

static bool
decode_rule_port_range(struct tcb *const tcp,
		       const kernel_ulong_t addr,
		       const unsigned int len,
		       const void *const opaque_data)
{
	struct_fib_rule_port_range range;

	if (len < sizeof(range))
		return false;
	else if (!umove_or_printaddr(tcp, addr, &range)) {
		PRINT_FIELD_U("{", range, start);
		PRINT_FIELD_U(", ", range, end);
		tprints("}");
	}

	return true;
}

static const nla_decoder_t fib_rule_hdr_nla_decoders[] = {
	[FRA_DST]			= decode_rule_addr,
	[FRA_SRC]			= decode_rule_addr,
	[FRA_IIFNAME]			= decode_nla_str,
	[FRA_GOTO]			= decode_nla_u32,
	[FRA_PRIORITY]			= decode_nla_u32,
	[FRA_FWMARK]			= decode_nla_u32,
	[FRA_FLOW]			= decode_nla_u32,
	[FRA_TUN_ID]			= decode_nla_be64,
	[FRA_SUPPRESS_IFGROUP]		= decode_nla_u32,
	[FRA_SUPPRESS_PREFIXLEN]	= decode_nla_u32,
	[FRA_TABLE]			= decode_nla_rt_class,
	[FRA_FWMASK]			= decode_nla_u32,
	[FRA_OIFNAME]			= decode_nla_str,
	[FRA_PAD]			= NULL,
	[FRA_L3MDEV]			= decode_nla_u8,
	[FRA_UID_RANGE]			= decode_fib_rule_uid_range,
	[FRA_PROTOCOL]			= decode_nla_rt_proto,
	[FRA_IP_PROTO]			= decode_nla_ip_proto,
	[FRA_SPORT_RANGE]		= decode_rule_port_range,
	[FRA_DPORT_RANGE]		= decode_rule_port_range,
};

DECL_NETLINK_ROUTE_DECODER(decode_fib_rule_hdr)
{
	struct_fib_rule_hdr msg = { .family = family };
	size_t offset = sizeof(msg.family);
	bool decode_nla = false;

	PRINT_FIELD_XVAL("{", msg, family, addrfams, "AF_???");

	tprints(", ");
	if (len >= sizeof(msg)) {
		if (!umoven_or_printaddr(tcp, addr + offset,
					 sizeof(msg) - offset,
					 (char *) &msg + offset)) {
			PRINT_FIELD_U("", msg, dst_len);
			PRINT_FIELD_U(", ", msg, src_len);
			PRINT_FIELD_FLAGS(", ", msg, tos,
					  ip_type_of_services, "IPTOS_TOS_???");
			PRINT_FIELD_XVAL(", ", msg, table,
					 routing_table_ids, "RT_TABLE_???");
			PRINT_FIELD_XVAL(", ", msg, action,
					 fib_rule_actions, "FR_ACT_???");
			PRINT_FIELD_FLAGS(", ", msg, flags,
					  fib_rule_flags, "FIB_RULE_???");
			decode_nla = true;
		}
	} else
		tprints("...");
	tprints("}");

	offset = NLMSG_ALIGN(sizeof(msg));
	if (decode_nla && len > offset) {
		tprints(", ");
		decode_nlattr(tcp, addr + offset, len - offset,
			      rtnl_rule_attrs, "FRA_???",
			      fib_rule_hdr_nla_decoders,
			      ARRAY_SIZE(fib_rule_hdr_nla_decoders), &msg);
	}
}
