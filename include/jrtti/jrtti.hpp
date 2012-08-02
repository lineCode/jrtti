#ifndef jrttiH
#define jrttiH

#include <map>
#include <typeinfo>
#include "exception.hpp"
#include "annotations.hpp"

namespace jrtti {
	typedef std::map< void *, std::string > AddressRefMap;
	typedef std::map< std::string, void * > NameRefMap;

	class Error;
	class Metatype;
	class Reflector;
//	Metatype &	getType(std::string name);
	std::string demangle( const std::string& name );
	template< typename T > Metatype& getType();
	template< typename C > class Metacollection;
	template <typename C> Metacollection<C>& declareCollection( const Annotations& annotations = Annotations() );

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

	/* *
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype by name in the reflection database
	 * \param name the Metatype name to look for
	 * \return the found Metatype.
	 * \throw Error if not found
	 */
/*	inline Metatype &
	getType(std::string name) {
		return Reflector::instance().getType(name);
	}
  */
	/**
	 * \brief Retrieve Metatype
	 *
	 * Looks for a Metatype of type T in the reflection database
	 * \tparam T the type to retrieve
	 * \return the found Metatype.
	 * \throws Error if not found
	 */
	template< typename T >
	inline Metatype &
	getType() {
		return Reflector::instance().getType< T >();
	}

	/**
	 * \brief Declare a user metaclass
	 *
	 * Declares a new user metaclass based on class C
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 */
	template <typename C>
	CustomMetaclass<C>&
	declare( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declare<C>( annotations );
	}
	
	/* *
	 * \brief Declare a user metaclass
	 *
	 * Declares a new user metaclass based on class C and assigns an alias name
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \param alias the alias name for the class
	 * \return this to chain calls
	 */
/*	template <typename C>
	CustomMetaclass<C>&
	declare( std::string alias, const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declare<C>( alias, annotations );
	}
*/
	/**
	 * \brief Declare an abstract user metaclass
	 *
	 * Declares a new abstract user metaclass based on class C
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 */
	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareAbstract<C>( annotations );
	}

	/* *
	 * \brief Declare an abstract user metaclass
	 *
	 * Declares a new abstract user metaclass based on class C and assigns an alias name
	 * \tparam C the class to declare
	 * \param alias the alias name for the class
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 */
/*	template <typename C>
	CustomMetaclass<C, boost::true_type>&
	declareAbstract( std::string alias, const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareAbstract<C>( alias, annotations );
	}
*/
	/**
	 * \brief Declare a collection
	 *
	 * Declares a new Metacollection based on collection C.
	 * A collection is a secuence of objects, as STL containers
	 * \tparam C the class to declare
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 */
	template <typename C>
	Metacollection<C>&
	declareCollection( const Annotations& annotations ) {
		return Reflector::instance().declareCollection<C>( annotations );
	}

	/* *
	 * \brief Declare a collection
	 *
	 * Declares a new Metacollection based on collection C  and assigns an alias name.
	 * A collection is a secuence of objects, as STL containers
	 * \tparam C the class to declare
	 * \param alias the alias name for the class
	 * \param annotations Annotation associated to this metaclass
	 * \return this to chain calls
	 */
/*	template <typename C>
	Metacollection<C>&
	declareCollection( std::string alias, const Annotations& annotations = Annotations() ) {
		return Reflector::instance().declareCollection<C>( alias, annotations );
	}
*/
	inline
	std::string
	demangle( const std::string& name ) {
		return Reflector::instance().demangle( name );
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
