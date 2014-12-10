

#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage_bgl;


int
main()
{
    Environment environment(true);

    Storage storage(environment);

    for (const string& name : storage.get_devicegraph_names())
	cout << name << endl;
    cout << endl;

    const Devicegraph* probed = storage.get_probed();

    probed->check();
    probed->print_graph();
    probed->write_graphviz("probe1");
    probed->save("probe1.info");

    const Devicegraph* current = storage.get_current();

    current->check();
    current->print_graph();
}
