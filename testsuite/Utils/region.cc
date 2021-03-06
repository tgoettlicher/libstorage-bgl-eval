
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_output)
{
    Region region(2048, 1603584, 512);

    ostringstream out;
    out << region;

    BOOST_CHECK_EQUAL(out.str(), "[2048, 1603584, 512 B]");
}


BOOST_AUTO_TEST_CASE(test_block_size_512)
{
    Region region(2048, 1603584, 512);

    BOOST_CHECK_EQUAL(region.get_start(), 2048);
    BOOST_CHECK_EQUAL(region.get_length(), 1603584);
    BOOST_CHECK_EQUAL(region.get_end(), 1605631);

    BOOST_CHECK_EQUAL(region.get_block_size(), 512);

    BOOST_CHECK_EQUAL(region.to_kb(region.get_start()), 1024);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_length()), 801792);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_end()), 802815);
}


BOOST_AUTO_TEST_CASE(test_block_size_4096)
{
    Region region(256, 65280, 4096);

    BOOST_CHECK_EQUAL(region.get_start(), 256);
    BOOST_CHECK_EQUAL(region.get_length(), 65280);
    BOOST_CHECK_EQUAL(region.get_end(), 65535);

    BOOST_CHECK_EQUAL(region.get_block_size(), 4096);

    BOOST_CHECK_EQUAL(region.to_kb(region.get_start()), 1024);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_length()), 261120);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_end()), 262140);
}


BOOST_AUTO_TEST_CASE(test_comparisons)
{
    // Four regions with block layout:
    //
    // aaaaaaaaaa
    //           bbbbbbbbbb
    //      cccccccccc
    // dddddddddddddddddddd

    Region a(0, 10, 1);
    Region b(10, 10, 1);
    Region c(5, 10, 1);
    Region d(0, 20, 1);

    BOOST_CHECK(a == a);
    BOOST_CHECK(a != b);

    BOOST_CHECK(a < b);
    BOOST_CHECK(c > d);

    BOOST_CHECK(a.inside(d));
    BOOST_CHECK(!a.inside(c));

    BOOST_CHECK(a.intersect(c));
    BOOST_CHECK(!a.intersect(b));

    BOOST_CHECK(a.intersection(c) == Region(5, 5, 1));
    BOOST_CHECK_THROW(a.intersection(b), NoIntersection);
}


BOOST_AUTO_TEST_CASE(test_different_block_size)
{
    Region a(0, 100, 1);
    Region b(0, 50, 2);

    BOOST_CHECK_THROW((void)(a == b), DifferentBlockSizes);
}


BOOST_AUTO_TEST_CASE(test_big_numbers)
{
    unsigned long long EiB = 1ULL << (10 * 6);

    Region kb(0, 1ULL << (10 * 5), 1ULL << (10 * 1));

    BOOST_CHECK_EQUAL(kb.to_kb(kb.get_length()), EiB / 1024);
    BOOST_CHECK_EQUAL(byte_to_humanstring(kb.to_kb(kb.get_length()) * 1024, true, 1, false), "1.0 EiB");

    Region mb(0, 1ULL << (10 * 4), 1ULL << (10 * 2));

    BOOST_CHECK_EQUAL(mb.to_kb(mb.get_length()), EiB / 1024);
    BOOST_CHECK_EQUAL(byte_to_humanstring(mb.to_kb(mb.get_length()) * 1024, true, 1, false), "1.0 EiB");
}
