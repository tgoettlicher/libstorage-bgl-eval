#ifndef STORAGE_DEVICE_IMPL_H
#define STORAGE_DEVICE_IMPL_H


#include <libxml/tree.h>
#include <type_traits>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/ActiongraphImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{

    using namespace std;


    class SystemInfo;


    template <typename Type> struct DeviceTraits {};


    template <typename Type> Type* to_device_of_type(Device* device);
    template <typename Type> const Type* to_device_of_type(const Device* device);


    // abstract class

    class Device::Impl
    {
    public:

	virtual ~Impl() {}

	virtual Impl* clone() const = 0;

	virtual const char* get_classname() const = 0;

	virtual string get_displayname() const = 0;

	virtual void save(xmlNode* node) const = 0;

	bool operator==(const Impl& rhs) const;
	bool operator!=(const Impl& rhs) const { return !(*this == rhs); }

	sid_t get_sid() const { return sid; }

	void set_devicegraph_and_vertex(Devicegraph* devicegraph,
					Devicegraph::Impl::vertex_descriptor vertex);

	Devicegraph* get_devicegraph();
	const Devicegraph* get_devicegraph() const;

	Devicegraph::Impl::vertex_descriptor get_vertex() const;

	Device* get_device() { return devicegraph->get_impl()[vertex]; }
	const Device* get_device() const { return devicegraph->get_impl()[vertex]; }

	const map<string, string>& get_userdata() const { return userdata; }
	void set_userdata(const map<string, string>& userdata) { Impl::userdata = userdata; }

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const;

	virtual bool equal(const Impl& rhs) const = 0;
	virtual void log_diff(std::ostream& log, const Impl& rhs) const = 0;

	virtual void print(std::ostream& out) const = 0;

	virtual Text do_create_text(bool doing) const;
	virtual void do_create() const;

	virtual Text do_delete_text(bool doing) const;
	virtual void do_delete() const;

	template<typename Type>
	Type* get_single_child_of_type()
	{
	    static_assert(!is_const<Type>::value, "Type must not be const");

	    Devicegraph::Impl& devicegraph_impl = get_devicegraph()->get_impl();

	    Device* tmp = devicegraph_impl[devicegraph_impl.child(get_vertex())];

	    return to_device_of_type<Type>(tmp);
	}

	template<typename Type>
	const Type* get_single_child_of_type() const
	{
	    static_assert(is_const<Type>::value, "Type must be const");

	    const Devicegraph::Impl& devicegraph_impl = get_devicegraph()->get_impl();

	    const Device* tmp = devicegraph_impl[devicegraph_impl.child(get_vertex())];

	    return to_device_of_type<Type>(tmp);
	}

    protected:

	Impl();

	Impl(const xmlNode* node);

	virtual void probe(SystemInfo& systeminfo);

    private:

	static sid_t global_sid;

	sid_t sid;

	Devicegraph* devicegraph;
	Devicegraph::Impl::vertex_descriptor vertex;

	map<string, string> userdata;

    };


    template <typename Type>
    bool is_device_of_type(const Device* device)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(device);

	return dynamic_cast<const Type*>(device);
    }


    template <typename Type>
    Type* to_device_of_type(Device* device)
    {
	static_assert(!std::is_const<Type>::value, "Type must not be const");

	ST_CHECK_PTR(device);

	Type* tmp = dynamic_cast<Type*>(device);
	if (!tmp)
	    ST_THROW(DeviceHasWrongType(device->get_impl().get_classname(),
					DeviceTraits<Type>::classname));

	return tmp;
    }


    template <typename Type>
    const Type* to_device_of_type(const Device* device)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(device);

	const Type* tmp = dynamic_cast<const Type*>(device);
	if (!tmp)
	    ST_THROW(DeviceHasWrongType(device->get_impl().get_classname(),
					DeviceTraits<typename std::remove_const<Type>::type>::classname));

	return tmp;
    }

}

#endif
