#ifndef COMMON_HPP
#define COMMON_HPP

#include <type_traits>
#include <random>
#include "memory/memory.hpp"
//#include "memory/ExtPreAlloc.hpp"
//#include "Vector/map_vector.hpp"

namespace std
{
	// We need the definition of std::to_string that work on string

	static std::string to_string(std::string s)
	{
		return s;
	}
}


//! Void structure
template<typename> struct Void
{
	//! define void type
	typedef void type;
};

template<typename T, typename Sfinae = void>
struct has_attributes: std::false_type {};


/*! \brief has_attributes check if a type has defined an
 * internal structure with attributes
 *
 * ### Example
 * \snippet util.hpp Declaration of struct with attributes and without
 * \snippet util.hpp Check has_attributes
 *
 * return true if T::attributes::name[0] is a valid expression
 * and produce a defined type
 *
 */
template<typename T>
struct has_attributes<T, typename Void<decltype( T::attributes::name[0] )>::type> : std::true_type
{};

template<typename T, typename Sfinae = void>
struct has_typedef_type: std::false_type {};

/*! \brief has_typedef_type check if a typedef ... type inside the structure is
 *         defined
 *
 * ### Example
 *
 * \snippet util.hpp Check has_typedef_type
 *
 * return true if T::type is a valid type
 *
 */
template<typename T>
struct has_typedef_type<T, typename Void< typename T::type>::type> : std::true_type
{};

template<typename T, typename Sfinae = void>
struct has_data: std::false_type {};

/*! \brief has_data check if a type has defined a member data
 *
 * ### Example
 *
 * \snippet util.hpp Check has_data
 *
 * return true if T::type is a valid type
 *
 */
template<typename T>
struct has_data<T, typename Void<decltype( T::data )>::type> : std::true_type
{};

template<typename T, typename Sfinae = void>
struct has_posMask: std::false_type {};

/*! \brief has_data check if a type has defined a member data
 *
 * ### Example
 *
 * \snippet util.hpp Check has_data
 *
 * return true if T::type is a valid type
 *
 */
template<typename T>
struct has_posMask<T, typename Void<decltype( T::stag_mask )>::type> : std::true_type
{};

/*! \brief check if T::type and T.data has the same type
 *
 * \tparam i when different from 0 a check is performed otherwise not, the reason of
 *           this is that the typedef and data could also not exist producing
 *           compilation error, this flag avoid this, it perform the check only if it
 *           is safe
 *
 * \tparam T
 *
 * ### Example
 *
 * \snippet util.hpp Check is_typedef_and_data_same
 *
 * return true if the type of T::data is the same of T::type, false otherwise
 *
 */
template<bool cond, typename T>
struct is_typedef_and_data_same
{
	enum
	{
		value = std::is_same<decltype(T().data),typename T::type>::value
	};
};


template<typename T>
struct is_typedef_and_data_same<false,T>
{
	enum
	{
		value = false
	};
};


template<typename T, typename Sfinae = void>
struct has_noPointers: std::false_type {};


/*! \brief has_noPointers check if a type has defined a
 * method called noPointers
 *
 * ### Example
 *
 * \snippet util.hpp Check no pointers
 *
 * return true if T::noPointers() is a valid expression (function pointers)
 * and produce a defined type
 *
 */
template<typename T>
struct has_noPointers<T, typename Void<decltype( T::noPointers() )>::type> : std::true_type
{};

template<typename ObjType, typename Sfinae = void>
struct has_Pack: std::false_type {};


/*! \brief has_Pack check if a type has defined a
 * method called Pack
 *
 * ### Example
 *
 * \snippet util.hpp Check for 'pack'
 *
 * return true if T::pack() is a valid expression (function pointers)
 * and produce a defined type
 *
 */
template<typename ObjType>
struct has_Pack<ObjType, typename Void<decltype( ObjType::pack() )>::type> : std::true_type
{};


/*template<bool cond>
struct nested_pack_cond
{
	template<int ... prp> void pack(ExtPreAlloc<Memory> & mem, openfpm::vector<T> & obj, Pack_stat & sts)
    {
               for (int i = 0; i < obj.size(); i++) {
                   T ele = obj.get(i);
                   ele.pack<prp...>(mem, sts);
    }
};

template<false>
struct nested_pack_cond
{};*/


#endif
