#ifndef jrttiH
#define jrttiH

#include <map>
#include "exception.hpp"

namespace jrtti {
	typedef std::map< void *, std::string > AddressRefMap;
	typedef std::map< std::string, void * > NameRefMap;

	class Error;
	class Metatype;
	class Reflector;
	Metatype &	getType(std::string name);
	Error	error(std::string message);

	AddressRefMap&	_addressRefMap();
	NameRefMap&	_nameRefMap();
}

#include "reflector.hpp"

/**
 * \brief jrtti top level functions
 */
namespace jrtti {
	inline Error
	error(std::string message)	{
		return Error(message);
	}

	/**
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype by name in the reflection database
	 * \param name the Metatype name to look for
	 * \return the found Metatype.
	 * \throws if not found
	 */
	inline Metatype &
	getType(std::string name) {
		return Reflector::instance().getType(name);
	}

	/**
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype of type T in the reflection database
	 * \return the found Metatype.
	 * \throws if not found
	 */
	template< typename T >
	inline Metatype &
	getType() {
		return Reflector::instance().getType< T >();
	}

	/**
	 * \brief Get the name of class C
	 * \return the name of class C
	 */
	template <typename C>
	std::string
	nameOf(){
		return Reflector::instance().nameOf<C>();
	}

	/**
	 * \brief Gives an alias name
	 *
	 * Set an alias name for class C
	 * \param new_name the alias name for class C
	 */
	template <typename C>
	void
	alias( const std::string& new_name ) {
		Reflector::instance().alias<C>(new_name);
	}

	/**
	 * \brief Declare a user metaclass
	 *
	 * Declares a new user metaclass based on class C
	 *
	 * \return this to chain calls
	 */
	template <typename C>
	CustomMetaclass<C>&
	declare( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declare<C>( annotations );
	}

	/**
	 * \brief Declare an abstract user metaclass
	 *
	 * Declares a new abstract user metaclass based on class C
	 *
	 * \return this to chain calls
	 */
	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareAbstract<C>( annotations );
	}

	/**
	 * \brief Declare a collection
	 *
	 * Declares a new Metacollection based on collection C.
	 * A collection is a secuence of objects, as STL containers
	 *
	 * \return this to chain calls
	 */
	template <typename C>
	MetaCollection<C>&
	declareCollection( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareCollection<C>( annotations );
	}

	inline
	AddressRefMap&
	_addressRefMap() {
		return Reflector::instance()._addressRefMap();
	}

	inline
	NameRefMap&
	_nameRefMap() {
		return Reflector::instance()._nameRefMap();
	}
}

#endif       // jrttiH
