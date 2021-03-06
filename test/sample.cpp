//---------------------------------------------------------------------------
#include "sample.h"

void declare()
{
	jrtti::declare<Date>()
						.property("d", &Date::d)
						.property("m", &Date::m)
						.property("y", &Date::y)
						.property("place", &Date::place);

	jrtti::declare< Rect >()
						.property( "tl", &Rect::tl )
						.property( "bl", &Rect::br );

#ifdef BOOST_NO_IS_ABSTRACT
	jrtti::declareAbstract<SampleBase>()			// declare abstract for generic compilers
						.property("intAbstract", &SampleBase::getIntAbstract)
						.property("intOverloaded", &SampleBase::getIntOverloaded);
#else
	jrtti::declare<SampleBase>()					// if compiler supports boost::is_abstract you can declare an abstract class directly with jrtti::declare
						.property("intAbstract", &SampleBase::getIntAbstract)
						.property("intOverloaded", &SampleBase::getIntOverloaded);
#endif

	jrtti::declare<Sample>( jrtti::Annotations() << new GUIAnnotation( "sample.ico" ) )
				.derivesFrom<SampleBase>()
						.property("intMember", &Sample::intMember,
									jrtti::Annotations() << new jrtti::NoStreamable() )
						.property("testDouble", &Sample::setDoubleProp, &Sample::getDoubleProp,
									jrtti::Annotations() << new GUIAnnotation( "test.ico", false, true ) )
						.property("point", &Sample::setByPtrProp, &Sample::getByPtrProp,
									jrtti::Annotations() << new MenuAnnotation( "Entry_1" ) << new MenuAnnotation( "Entry_2" ) << new MenuAnnotation( "Entry_n" ) )
						.property("date", &Sample::setByValProp, &Sample::getByValProp )
						.property("refToDate", &Sample::getByRefProp)
						.property("testStr", &Sample::setStdStringProp,&Sample::getStdStringProp)
						.property("testRO", &Sample::testIntFunc)
						.property("testBool", &Sample::setBool, &Sample::getBool)
						.collection("collection", /*&Sample::setCollection, */&Sample::getCollection, jrtti::Annotations() << new jrtti::ForceStreamLoadable() )
						.property("circularRef", &Sample::circularRef )
						.property("memoryDump", jrtti::Annotations() << new jrtti::StringifyDelegate<Sample>( &Sample::stringifier, &Sample::deStringifier) )
						.property("onlySetter", &Sample::setDoubleProp )

						.method<void>("testMethod", &Sample::testFunc,
									jrtti::Annotations() << new GUIAnnotation( "method.ico", false, false ) )
						.method<int>("testIntMethod", &Sample::testIntFunc)
						.method<double,double>("testSquare", &Sample::testSquare)
						.method<double,int,double>("testSum", &Sample::testSum);

	jrtti::declare<Point>()
						.property("x", &Point::x)
						.property("y", &Point::y);

	jrtti::declare<SampleDerived>()
				.derivesFrom<Sample>();

	//jrtti::declareCollection< Sample::Collection >();
}

void useCase() {
	Sample s;
	Point p;
	Date d;

	//populate s
	p.x = 30; p.y = 60;
	d.d = 1; d.m = 5; d.y = 2012; d.place = p;
	s.setByValProp( d );
	s.circularRef = &s; s.setDoubleProp( 344.23 ); s.setStdStringProp( "Hello!!!" );
	
	//set the property intMember of s object to 15
	jrtti::metatype< Sample >().property( "intMember" ).set( &s, 15 );
	std::cout << s.intMember << " == " << 15 << std::endl;

	//retrieve the value of intMember from s object
	int i = jrtti::metatype< Sample >().property( "intMember" ).get<int>( &s );
	std::cout << s.intMember << " == " << i << std::endl;

	//same as above using braket operator
	i = jrtti::metatype< Sample >()[ "intMember" ].get<int>( &s );
	std::cout << s.intMember << " == " << i << std::endl;

	//getting a Metatype object
	jrtti::Metatype & mt = jrtti::metatype< Sample >();

	//and working with it
	p.x = 23;
	mt[ "point" ].set( &s, &p );
	std::cout << 23 << " == " << s.getByPtrProp()->x << std::endl;

	//call a method without parameters returning void
	mt.call<void>( "testMethod", &s );

	//call a method returning int and having two parameters
	double f = mt.call<double,Sample,int,double>( "testSum", &s, 3, 8 );
	std::cout << 3+8 << " == " << f << std::endl;
	//or
	f = mt.call<double>( "testSum", &s, 3, 8.0 );
	std::cout << 3+8 << " == " << f << std::endl;


	//set value from a fully qualified path
	mt.apply( &s, "date.place.x", 25.0 );
	//get value from a fully qualified path
	f = mt.eval<double>( &s, "date.place.x" );
	std::cout << 25 << " == " << f << std::endl;

	//get a string representation of s object
	std::string contens = mt.toStr( &s );
	//get a streamable string representation of s objecy
	contens = mt.toStr( &s, true );
	//and set the s object from a string representation
	mt.fromStr( &s, contens );
	std::cout << contens << std::endl;
}

