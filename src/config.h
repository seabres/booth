/* 
 * Copyright (C) 2011 Jiaju Zhang <jjzhang@suse.de>
 * Copyright (C) 2013-2014 Philipp Marek <philipp.marek@linbit.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>
#include "booth.h"
#include "raft.h"
#include "transport.h"


/** @{ */
/** Definitions for in-RAM data. */

#define MAX_NODES	16
#define TICKET_ALLOC	16



struct ticket_config {
	/** \name Configuration items.
	 * @{ */
	/** Name of ticket. */
	boothc_ticket name;

	/** How many seconds a term lasts (if not refreshed). */
	int term_duration;

	/** Network related timeouts. */
	int timeout;

	/** Retries before giving up. */
	int retries;

	/** If >0, time to wait for a site to get fenced.
	 * The ticket may be acquired after that timespan by
	 * another site. */
	int acquire_after; /* TODO: needed? */


	/* Program to ask whether it makes sense to
	 * acquire the ticket */
	char *ext_verifier;

	/** Node weights. */
	int weight[MAX_NODES];
	/** @} */


	/** \name Runtime values.
	 * @{ */
	/** Current state. */
	server_state_e state;

	/** When something has to be done */
	struct timeval next_cron;

	/** Current leader. This is effectively the log[] in Raft. */
	struct booth_site *leader;

	/** Timestamp of leadership expiration */
	time_t term_expires;
	/** End of election period */
	time_t election_end;
	struct booth_site *voted_for;


	/** Who the various sites vote for.
	 * NO_OWNER = no vote yet. */
	struct booth_site *votes_for[MAX_NODES];
	/* bitmap */
	uint64_t votes_received;

	/** Last voting round that was seen. */
	uint32_t current_term;

	/** Do ticket updates whenever we get enough heartbeats.
	 * But do that only once.
	 * This is reset to 0 whenever we broadcast heartbeat and set
	 * to 1 once enough acks are received.
	 */
	uint32_t majority_acks_received;
	/** @} */


	/** */
	uint32_t commit_index;

	/** */
	uint32_t last_applied;
	uint32_t next_index[MAX_NODES];
	uint32_t match_index[MAX_NODES];


	uint64_t hb_received;
	time_t hb_sent_at;

	/** \name Needed while proposals are being done.
	 * @{ */
	/** Whom to vote for the next time.
	 * Needed to push a ticket to someone else. */



#if 0
	/** Bitmap of sites that acknowledge that state. */
	uint64_t proposal_acknowledges;

	/** When an incompletely acknowledged proposal gets done.
	 * If all peers agree, that happens sooner.
	 * See switch_state_to(). */
	struct timeval proposal_switch;

	/** Timestamp of proposal expiration. */
	time_t proposal_expires;

#endif

	/** Number of send retries left.
	 * Used on the new owner.
	 * Starts at 0, counts up. */
	int retry_number;
	/** @} */
};


struct booth_config {
    char name[BOOTH_NAME_LEN];

    transport_layer_t proto;
    uint16_t port;

    /** Stores the OR of the individual host bitmasks. */
    uint64_t site_bits;

    char site_user[BOOTH_NAME_LEN];
    char site_group[BOOTH_NAME_LEN];
    char arb_user[BOOTH_NAME_LEN];
    char arb_group[BOOTH_NAME_LEN];
    uid_t uid;
    gid_t gid;

    int site_count;
    struct booth_site site[MAX_NODES];

    int ticket_count;
    int ticket_allocated;
    struct ticket_config *ticket;
};


extern struct booth_config *booth_conf;


int read_config(const char *path);

int check_config(int type);

int find_site_by_name(unsigned char *site, struct booth_site **node, int any_type);
int find_site_by_id(uint32_t site_id, struct booth_site **node);

const char *type_to_string(int type);


#include <stdio.h>
#define R(tk_) printf("## %12s:%3d state %s, term %d, index %d, leader %s\n", __FILE__, __LINE__, state_to_string(tk_->state), tk_->current_term, tk_->commit_index, site_string(tk_->leader))


#endif /* _CONFIG_H */
