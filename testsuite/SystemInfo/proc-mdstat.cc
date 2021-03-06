
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/Utils/Mockup.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_file("/proc/mdstat", input);

    ProcMdstat procmdstat;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << procmdstat;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"Personalities : [raid0] [raid1] [raid10] [raid6] [raid5] [raid4] ",
	"md0 : active raid1 sdb1[1] sda1[0]",
	"      8387520 blocks super 1.0 [2/2] [UU]",
	"      [====>................]  resync = 23.4% (1963200/8387520) finish=4.6min speed=23178K/sec",
	"      bitmap: 1/1 pages [4KB], 65536KB chunk",
	"",
	"unused devices: <none>"
    };

    vector<string> output = {
	"data[md0] -> md_type:raid1 super:1.0 size_k:8387520 devices:</dev/sda1 /dev/sdb1>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"Personalities : [raid1] [raid0] ",
	"md125 : active (auto-read-only) raid1 sda[1] sdb[0]",
	"      4194304 blocks super external:/md127/0 [2/2] [UU]",
	"      ",
	"md126 : active raid0 sda[1] sdb[0]",
	"      8378790 blocks super external:/md127/1 128k chunks",
	"      ",
	"md127 : inactive sdb[1](S) sda[0](S)",
	"      5104 blocks super external:imsm",
	"       ",
	"unused devices: <none>",
    };

    vector<string> output = {
	"data[md125] -> md_type:raid1 super:external:/md127/0 size_k:4194304 readonly devices:</dev/sdb /dev/sda> has_container container_name:md127 container_member:0",
	"data[md126] -> md_type:raid0 super:external:/md127/1 chunk_k:128 size_k:8378790 devices:</dev/sdb /dev/sda> has_container container_name:md127 container_member:1",
	"data[md127] -> md_type:unknown super:external:imsm size_k:5104 readonly inactive devices:<> spares:</dev/sda /dev/sdb> is_container"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"Personalities : [raid1] [raid0] ",
	"md125 : active raid1 sda[1] sdb[0]",
	"      7355904 blocks super external:/md127/1 [2/2] [UU]",
	"      ",
	"md126 : active raid0 sda[1] sdb[0]",
	"      1999872 blocks super external:/md127/0 512k chunks",
	"      ",
	"md127 : inactive sdb[1](S) sda[0](S)",
	"      65536 blocks super external:ddf",
	"       ",
	"unused devices: <none>",
    };

    vector<string> output = {
	"data[md125] -> md_type:raid1 super:external:/md127/1 size_k:7355904 devices:</dev/sdb /dev/sda> has_container container_name:md127 container_member:1",
	"data[md126] -> md_type:raid0 super:external:/md127/0 chunk_k:512 size_k:1999872 devices:</dev/sdb /dev/sda> has_container container_name:md127 container_member:0",
	"data[md127] -> md_type:unknown super:external:ddf size_k:65536 readonly inactive devices:<> spares:</dev/sda /dev/sdb> is_container"
    };

    check(input, output);
}
