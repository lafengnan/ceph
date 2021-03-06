// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2011 New Dream Network
 * Copyright (C) 2013 Cloudwatt <libre.licensing@cloudwatt.com>
 *
 * Author: Loic Dachary <loic@dachary.org>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "include/types.h"
#include "osd/osd_types.h"
#include "gtest/gtest.h"

#include <sstream>

TEST(hobject, prefixes0)
{
  uint32_t mask = 0xE947FA20;
  uint32_t bits = 12;
  int64_t pool = 0;

  set<string> prefixes_correct;
  prefixes_correct.insert(string("0000000000000000.02A"));

  set<string> prefixes_out(hobject_t::get_prefixes(bits, mask, pool));
  ASSERT_EQ(prefixes_out, prefixes_correct);
}

TEST(hobject, prefixes1)
{
  uint32_t mask = 0x0000000F;
  uint32_t bits = 6;
  int64_t pool = 20;

  set<string> prefixes_correct;
  prefixes_correct.insert(string("0000000000000014.F0"));
  prefixes_correct.insert(string("0000000000000014.F4"));
  prefixes_correct.insert(string("0000000000000014.F8"));
  prefixes_correct.insert(string("0000000000000014.FC"));

  set<string> prefixes_out(hobject_t::get_prefixes(bits, mask, pool));
  ASSERT_EQ(prefixes_out, prefixes_correct);
}

TEST(hobject, prefixes2)
{
  uint32_t mask = 0xDEADBEAF;
  uint32_t bits = 25;
  int64_t pool = 0;

  set<string> prefixes_correct;
  prefixes_correct.insert(string("0000000000000000.FAEBDA0"));
  prefixes_correct.insert(string("0000000000000000.FAEBDA2"));
  prefixes_correct.insert(string("0000000000000000.FAEBDA4"));
  prefixes_correct.insert(string("0000000000000000.FAEBDA6"));
  prefixes_correct.insert(string("0000000000000000.FAEBDA8"));
  prefixes_correct.insert(string("0000000000000000.FAEBDAA"));
  prefixes_correct.insert(string("0000000000000000.FAEBDAC"));
  prefixes_correct.insert(string("0000000000000000.FAEBDAE"));

  set<string> prefixes_out(hobject_t::get_prefixes(bits, mask, pool));
  ASSERT_EQ(prefixes_out, prefixes_correct);
}

TEST(hobject, prefixes3)
{
  uint32_t mask = 0xE947FA20;
  uint32_t bits = 32;
  int64_t pool = 0x23;

  set<string> prefixes_correct;
  prefixes_correct.insert(string("0000000000000023.02AF749E"));

  set<string> prefixes_out(hobject_t::get_prefixes(bits, mask, pool));
  ASSERT_EQ(prefixes_out, prefixes_correct);
}

TEST(hobject, prefixes4)
{
  uint32_t mask = 0xE947FA20;
  uint32_t bits = 0;
  int64_t pool = 0x23;

  set<string> prefixes_correct;
  prefixes_correct.insert(string("0000000000000023."));

  set<string> prefixes_out(hobject_t::get_prefixes(bits, mask, pool));
  ASSERT_EQ(prefixes_out, prefixes_correct);
}

TEST(hobject, prefixes5)
{
  uint32_t mask = 0xDEADBEAF;
  uint32_t bits = 1;
  int64_t pool = 0x34AC5D00;

  set<string> prefixes_correct;
  prefixes_correct.insert(string("0000000034AC5D00.1"));
  prefixes_correct.insert(string("0000000034AC5D00.3"));
  prefixes_correct.insert(string("0000000034AC5D00.5"));
  prefixes_correct.insert(string("0000000034AC5D00.7"));
  prefixes_correct.insert(string("0000000034AC5D00.9"));
  prefixes_correct.insert(string("0000000034AC5D00.B"));
  prefixes_correct.insert(string("0000000034AC5D00.D"));
  prefixes_correct.insert(string("0000000034AC5D00.F"));

  set<string> prefixes_out(hobject_t::get_prefixes(bits, mask, pool));
  ASSERT_EQ(prefixes_out, prefixes_correct);
}

TEST(pg_t, split)
{
  pg_t pgid(0, 0, -1);
  set<pg_t> s;
  bool b;

  s.clear();
  b = pgid.is_split(1, 1, &s);
  ASSERT_TRUE(!b);

  s.clear();
  b = pgid.is_split(2, 4, NULL);
  ASSERT_TRUE(b);
  b = pgid.is_split(2, 4, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(1u, s.size());
  ASSERT_TRUE(s.count(pg_t(2, 0, -1)));

  s.clear();
  b = pgid.is_split(2, 8, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(3u, s.size());
  ASSERT_TRUE(s.count(pg_t(2, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(4, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(6, 0, -1)));

  s.clear();
  b = pgid.is_split(3, 8, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(1u, s.size());
  ASSERT_TRUE(s.count(pg_t(4, 0, -1)));

  s.clear();
  b = pgid.is_split(6, 8, NULL);
  ASSERT_TRUE(!b);
  b = pgid.is_split(6, 8, &s);
  ASSERT_TRUE(!b);
  ASSERT_EQ(0u, s.size());

  pgid = pg_t(1, 0, -1);
  
  s.clear();
  b = pgid.is_split(2, 4, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(1u, s.size());
  ASSERT_TRUE(s.count(pg_t(3, 0, -1)));

  s.clear();
  b = pgid.is_split(2, 6, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(2u, s.size());
  ASSERT_TRUE(s.count(pg_t(3, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(5, 0, -1)));

  s.clear();
  b = pgid.is_split(2, 8, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(3u, s.size());
  ASSERT_TRUE(s.count(pg_t(3, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(5, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(7, 0, -1)));

  s.clear();
  b = pgid.is_split(4, 8, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(1u, s.size());
  ASSERT_TRUE(s.count(pg_t(5, 0, -1)));

  s.clear();
  b = pgid.is_split(3, 8, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(3u, s.size());
  ASSERT_TRUE(s.count(pg_t(3, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(5, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(7, 0, -1)));

  s.clear();
  b = pgid.is_split(6, 8, &s);
  ASSERT_TRUE(!b);
  ASSERT_EQ(0u, s.size());

  pgid = pg_t(3, 0, -1);

  s.clear();
  b = pgid.is_split(7, 8, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(1u, s.size());
  ASSERT_TRUE(s.count(pg_t(7, 0, -1)));

  s.clear();
  b = pgid.is_split(7, 12, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(2u, s.size());
  ASSERT_TRUE(s.count(pg_t(7, 0, -1)));
  ASSERT_TRUE(s.count(pg_t(11, 0, -1)));

  s.clear();
  b = pgid.is_split(7, 11, &s);
  ASSERT_TRUE(b);
  ASSERT_EQ(1u, s.size());
  ASSERT_TRUE(s.count(pg_t(7, 0, -1)));

}

TEST(pg_missing_t, constructor)
{
  pg_missing_t missing;
  EXPECT_EQ((unsigned int)0, missing.num_missing());
  EXPECT_FALSE(missing.have_missing());
}

TEST(pg_missing_t, have_missing)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  pg_missing_t missing;
  EXPECT_FALSE(missing.have_missing());
  missing.add(oid, eversion_t(), eversion_t());
  EXPECT_TRUE(missing.have_missing());
}

TEST(pg_missing_t, swap)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  pg_missing_t missing;
  EXPECT_FALSE(missing.have_missing());
  missing.add(oid, eversion_t(), eversion_t());
  EXPECT_TRUE(missing.have_missing());

  pg_missing_t other;
  EXPECT_FALSE(other.have_missing());

  other.swap(missing);
  EXPECT_FALSE(missing.have_missing());  
  EXPECT_TRUE(other.have_missing());  
}

TEST(pg_missing_t, is_missing)
{
  // pg_missing_t::is_missing(const hobject_t& oid) const
  {
    hobject_t oid(object_t("objname"), "key", 123, 456, 0);
    pg_missing_t missing;
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add(oid, eversion_t(), eversion_t());
    EXPECT_TRUE(missing.is_missing(oid));
  }

  // bool pg_missing_t::is_missing(const hobject_t& oid, eversion_t v) const
  {
    hobject_t oid(object_t("objname"), "key", 123, 456, 0);
    pg_missing_t missing;
    eversion_t need(10,5);
    EXPECT_FALSE(missing.is_missing(oid, eversion_t()));
    missing.add(oid, need, eversion_t());
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_FALSE(missing.is_missing(oid, eversion_t()));
    EXPECT_TRUE(missing.is_missing(oid, need));
  }
}

TEST(pg_missing_t, have_old)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  pg_missing_t missing;
  EXPECT_EQ(eversion_t(), missing.have_old(oid));
  missing.add(oid, eversion_t(), eversion_t());
  EXPECT_EQ(eversion_t(), missing.have_old(oid));
  eversion_t have(1,1);
  missing.revise_have(oid, have);
  EXPECT_EQ(have, missing.have_old(oid));
}

TEST(pg_missing_t, add_next_event)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  hobject_t oid_other(object_t("other"), "key", 9123, 9456, 0);
  eversion_t version(10,5);
  eversion_t prior_version(3,4);
  pg_log_entry_t sample_e(pg_log_entry_t::DELETE, oid, version, prior_version,
			  osd_reqid_t(entity_name_t::CLIENT(777), 8, 999), utime_t(8,9));

  // new object (MODIFY)
  {
    pg_missing_t missing;
    pg_log_entry_t e = sample_e;

    e.op = pg_log_entry_t::MODIFY;
    e.prior_version = eversion_t();
    EXPECT_TRUE(e.is_update());
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(eversion_t(), missing.missing[oid].have);
    EXPECT_EQ(oid, missing.rmissing[e.version.version]);
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());

    // adding the same object replaces the previous one
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());
  }
  
  // new object (CLONE)
  {
    pg_missing_t missing;
    pg_log_entry_t e = sample_e;

    e.op = pg_log_entry_t::CLONE;
    e.prior_version = eversion_t();
    EXPECT_TRUE(e.is_clone());
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(eversion_t(), missing.missing[oid].have);
    EXPECT_EQ(oid, missing.rmissing[e.version.version]);
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());

    // adding the same object replaces the previous one
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());
  }

  // existing object (MODIFY)
  {
    pg_missing_t missing;
    pg_log_entry_t e = sample_e;

    e.op = pg_log_entry_t::MODIFY;
    e.prior_version = eversion_t();
    EXPECT_TRUE(e.is_update());
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(eversion_t(), missing.missing[oid].have);
    EXPECT_EQ(oid, missing.rmissing[e.version.version]);
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());

    // adding the same object with a different version
    e.prior_version = prior_version;
    missing.add_next_event(e);
    EXPECT_EQ(eversion_t(), missing.missing[oid].have);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());
  }
  
  // object with prior version (MODIFY)
  {
    pg_missing_t missing;
    pg_log_entry_t e = sample_e;

    e.op = pg_log_entry_t::MODIFY;
    EXPECT_TRUE(e.is_update());
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));
    EXPECT_EQ(prior_version, missing.missing[oid].have);
    EXPECT_EQ(version, missing.missing[oid].need);
    EXPECT_EQ(oid, missing.rmissing[e.version.version]);
    EXPECT_EQ(1U, missing.num_missing());
    EXPECT_EQ(1U, missing.rmissing.size());
  }

  // obsolete (BACKLOG)
  {
    pg_missing_t missing;
    pg_log_entry_t e = sample_e;

    e.op = pg_log_entry_t::BACKLOG;
    EXPECT_TRUE(e.is_backlog());
    EXPECT_FALSE(missing.is_missing(oid));
    EXPECT_THROW(missing.add_next_event(e), FailedAssertion);
  }
  
  // adding a DELETE matching an existing event
  {
    pg_missing_t missing;
    pg_log_entry_t e = sample_e;

    e.op = pg_log_entry_t::MODIFY;
    EXPECT_TRUE(e.is_update());
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add_next_event(e);
    EXPECT_TRUE(missing.is_missing(oid));

    e.op = pg_log_entry_t::DELETE;
    EXPECT_TRUE(e.is_delete());
    missing.add_next_event(e);
    EXPECT_FALSE(missing.have_missing());
  }
}

TEST(pg_missing_t, revise_need)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  pg_missing_t missing;
  // create a new entry
  EXPECT_FALSE(missing.is_missing(oid));
  eversion_t need(10,10);
  missing.revise_need(oid, need);
  EXPECT_TRUE(missing.is_missing(oid));
  EXPECT_EQ(eversion_t(), missing.missing[oid].have);
  EXPECT_EQ(need, missing.missing[oid].need);
  // update an existing entry and preserve have
  eversion_t have(1,1);
  missing.revise_have(oid, have);
  eversion_t new_need(10,12);
  EXPECT_EQ(have, missing.missing[oid].have);
  missing.revise_need(oid, new_need);
  EXPECT_EQ(have, missing.missing[oid].have);
  EXPECT_EQ(new_need, missing.missing[oid].need);
}

TEST(pg_missing_t, revise_have)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  pg_missing_t missing;
  // a non existing entry means noop
  EXPECT_FALSE(missing.is_missing(oid));
  eversion_t have(1,1);
  missing.revise_have(oid, have);
  EXPECT_FALSE(missing.is_missing(oid));
  // update an existing entry
  eversion_t need(10,12);
  missing.add(oid, need, have);
  EXPECT_TRUE(missing.is_missing(oid));
  eversion_t new_have(2,2);
  EXPECT_EQ(have, missing.missing[oid].have);
  missing.revise_have(oid, new_have);
  EXPECT_EQ(new_have, missing.missing[oid].have);
  EXPECT_EQ(need, missing.missing[oid].need);
}

TEST(pg_missing_t, add)
{
  hobject_t oid(object_t("objname"), "key", 123, 456, 0);
  pg_missing_t missing;
  EXPECT_FALSE(missing.is_missing(oid));
  eversion_t have(1,1);
  eversion_t need(10,10);
  missing.add(oid, need, have);
  EXPECT_TRUE(missing.is_missing(oid));
  EXPECT_EQ(have, missing.missing[oid].have);
  EXPECT_EQ(need, missing.missing[oid].need);
}

TEST(pg_missing_t, rm)
{
  // void pg_missing_t::rm(const hobject_t& oid, eversion_t v)
  {
    hobject_t oid(object_t("objname"), "key", 123, 456, 0);
    pg_missing_t missing;
    EXPECT_FALSE(missing.is_missing(oid));
    epoch_t epoch = 10;
    eversion_t need(epoch,10);
    missing.add(oid, need, eversion_t());
    EXPECT_TRUE(missing.is_missing(oid));
    // rm of an older version is a noop
    missing.rm(oid, eversion_t(epoch / 2,20));
    EXPECT_TRUE(missing.is_missing(oid));
    // rm of a later version removes the object
    missing.rm(oid, eversion_t(epoch * 2,20));
    EXPECT_FALSE(missing.is_missing(oid));
  }
  // void pg_missing_t::rm(const std::map<hobject_t, pg_missing_t::item>::iterator &m)
  {
    hobject_t oid(object_t("objname"), "key", 123, 456, 0);
    pg_missing_t missing;
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add(oid, eversion_t(), eversion_t());
    EXPECT_TRUE(missing.is_missing(oid));
    const std::map<hobject_t, pg_missing_t::item>::iterator m = missing.missing.find(oid);
    missing.rm(m);
    EXPECT_FALSE(missing.is_missing(oid));
  }
}

TEST(pg_missing_t, got)
{
  // void pg_missing_t::got(const hobject_t& oid, eversion_t v)
  {
    hobject_t oid(object_t("objname"), "key", 123, 456, 0);
    pg_missing_t missing;
    // assert if the oid does not exist
    EXPECT_THROW(missing.got(oid, eversion_t()), FailedAssertion);
    EXPECT_FALSE(missing.is_missing(oid));
    epoch_t epoch = 10;
    eversion_t need(epoch,10);
    missing.add(oid, need, eversion_t());
    EXPECT_TRUE(missing.is_missing(oid));
    // assert if that the version to be removed is lower than the version of the object
    EXPECT_THROW(missing.got(oid, eversion_t(epoch / 2,20)), FailedAssertion);
    // remove of a later version removes the object
    missing.got(oid, eversion_t(epoch * 2,20));
    EXPECT_FALSE(missing.is_missing(oid));
  }
  // void pg_missing_t::got(const std::map<hobject_t, pg_missing_t::item>::iterator &m)
  {
    hobject_t oid(object_t("objname"), "key", 123, 456, 0);
    pg_missing_t missing;
    EXPECT_FALSE(missing.is_missing(oid));
    missing.add(oid, eversion_t(), eversion_t());
    EXPECT_TRUE(missing.is_missing(oid));
    const std::map<hobject_t, pg_missing_t::item>::iterator m = missing.missing.find(oid);
    missing.got(m);
    EXPECT_FALSE(missing.is_missing(oid));
  }
}

TEST(pg_missing_t, split_into)
{
  uint32_t hash1 = 1;
  hobject_t oid1(object_t("objname"), "key1", 123, hash1, 0);
  uint32_t hash2 = 2;
  hobject_t oid2(object_t("objname"), "key2", 123, hash2, 0);
  pg_missing_t missing;
  missing.add(oid1, eversion_t(), eversion_t());
  missing.add(oid2, eversion_t(), eversion_t());  
  pg_t child_pgid;
  child_pgid.m_seed = 1;
  pg_missing_t child;  
  unsigned split_bits = 1;
  missing.split_into(child_pgid, split_bits, &child);
  EXPECT_TRUE(child.is_missing(oid1));
  EXPECT_FALSE(child.is_missing(oid2));
  EXPECT_FALSE(missing.is_missing(oid1));
  EXPECT_TRUE(missing.is_missing(oid2));
}

// Local Variables:
// compile-command: "cd .. ; make unittest_osd_types ; ./unittest_osd_types # --gtest_filter=pg_missing_t.constructor "
// End:
