#include <algorithm>
#include <fstream>
#include <time.h>
#include <gtest/gtest.h>
#include "test_jrtti.h"
#include "sample.h"


using namespace jrtti;

// To use a test fixture, derive a class from testing::Test.
class MetaTypeTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
						 // accessed from sub-classes.

	// virtual void SetUp() will be called before each test is run.  You
	// should define it if you need to initialize the varaibles.
	// Otherwise, this can be skipped.
	virtual void SetUp() {
		declare();
	}

	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.  Otherwise,
	// you don't have to provide it.
	//
	virtual void TearDown() {
		jrtti::Reflector::instance().clear();
	}

	Metatype & mClass(){
		return jrtti::metatype<Sample>();
	}

	Metatype & derivedClass(){
		return jrtti::metatype<SampleDerived>();
	}

	// Declares the variables your tests want to use.
	Sample sample;
	SampleDerived sampleDerived;
};

TEST_F(MetaTypeTest, InvertedDeclaration) {
	Point p;
	p.x = 45;
	p.y = 80;
	
	Rect r;
	r.tl = &p;
	r.br = &p;

	Date d;
	d.place.x = 12;
	d.place.y = 21;

	double x = jrtti::metatype< Rect >().eval<double>( &r, "tl.x" );
	EXPECT_EQ( x, r.tl->x );

	double y = jrtti::metatype< Date >().eval<double>( &d, "place.x" );
	EXPECT_EQ( y, d.place.x );
}

TEST_F(MetaTypeTest, DoubleType) {
	EXPECT_EQ("double", mClass()["testDouble"].metatype().name());
}

TEST_F(MetaTypeTest, DoubleAccessor) {

	sample.setDoubleProp(65.0);
	double result = boost::any_cast<double>(mClass()["testDouble"].get(&sample));
	EXPECT_EQ(65.0, result);
	// or use this shorter form
	double result2 = mClass()["testDouble"].get<double>(&sample);
	EXPECT_EQ(65.0, result2);
}

TEST_F(MetaTypeTest, AbstractAccessor) {
	int result = mClass()["intAbstract"].get<int>(&sample);

	EXPECT_EQ(34, result);
}

TEST_F(MetaTypeTest, OverloadedAccessor) {
	int result = mClass()["intOverloaded"].get<int>(&sample);

	EXPECT_EQ(87, result);
}

TEST_F(MetaTypeTest, DerivedOverloadedAccessor) {
	int result = derivedClass()["intOverloaded"].get<int>(&sampleDerived);

	EXPECT_EQ(43, result);
}

TEST_F(MetaTypeTest, DoubleMutator) {

	mClass()["testDouble"].set(&sample, 56.0);
	EXPECT_EQ(56, sample.getDoubleProp());
}

TEST_F(MetaTypeTest, IntMemberType) {

	EXPECT_EQ("int", mClass()["intMember"].metatype().name());
}

TEST_F(MetaTypeTest, BoolMutator) {
	sample.setBool( false );
	mClass()["testBool"].set(&sample, true);
	EXPECT_EQ(true, mClass()["testBool"].get<bool>(&sample));
}

TEST_F(MetaTypeTest, IntMemberAccessor) {

	sample.intMember = 123;
	int result = mClass()["intMember"].get<int>(&sample);
	EXPECT_EQ(123, result);
}

TEST_F(MetaTypeTest, IntMemberMutator) {
	sample.intMember = 123;
	mClass()["intMember"].set(&sample, 321);
	EXPECT_EQ(321, sample.intMember);
}

const std::string kHelloString = "Hello, \"world\"!\nThis is a new line with non printable char\x11";

TEST_F(MetaTypeTest, StdStringAccessor) {
	sample.setStdStringProp(kHelloString);
	std::string result = mClass()["testStr"].get<std::string>(&sample);

	EXPECT_EQ(kHelloString, result);
}

TEST_F(MetaTypeTest, StdStringMutator) {
	mClass()["testStr"].set(&sample, kHelloString);

	EXPECT_EQ(kHelloString, sample.getStdStringProp());
}

TEST_F(MetaTypeTest, ByValType) {
	EXPECT_EQ( "Date", mClass()["date"].metatype().name() );
}

TEST_F(MetaTypeTest, ByValAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	sample.setByValProp(d);

	Date result = mClass()["date"].get<Date>(&sample);

	EXPECT_TRUE(d == result);
}

TEST_F(MetaTypeTest, ByValMutator) {
	Date d;
	d.d = 9;
	d.m = 4;
	d.y = 2011;

	mClass()["date"].set(&sample, d);

	EXPECT_TRUE(d == sample.getByValProp());
}

TEST_F(MetaTypeTest, ByRefAccessor) {
	Date d;
	d.d = 1;
	d.m = 4;
	d.y = 2011;

	sample.setByValProp(d);

	Date& result = boost::any_cast<  boost::reference_wrapper< Date > >( mClass()["refToDate"].get(&sample) ).get();
	result.d = 31;

	EXPECT_EQ(sample.getByRefProp().d, result.d);
}

TEST_F(MetaTypeTest, ByPtrAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByPtrProp(p);
//TODO: improve access by proper type instead void *
	Point * result = mClass()["point"].get<Point *>(&sample);

	EXPECT_TRUE(p == result);
}

TEST_F(MetaTypeTest, ByPtrMutator) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;

	mClass()["point"].set(&sample, p);

	EXPECT_TRUE(p == sample.getByPtrProp());
}

TEST_F(MetaTypeTest, NestedByRefAccessor) {
	Point * p = new Point();
	p->x = 45;
	p->y = 80;
	sample.setByPtrProp(p);

	double result = mClass().eval<double>(&sample, "point.x");

	EXPECT_EQ(p->x, result);
}

TEST_F(MetaTypeTest, NestedByValAccessor) {
	Date d;
	d.d = 9;
	d.m = 4;
	d.y = 2011;
	d.place.x = 40.0;
	sample.setByValProp(d);

	int result = boost::any_cast<int>(mClass().eval(&sample, "date.y"));
	EXPECT_EQ(sample.getByValProp().y, result);

	d.y = 3056;
	sample.setByValProp(d);

	// this way is shorter and clearer
	result = mClass().eval<int>(&sample, "date.y");
	EXPECT_EQ(sample.getByValProp().y, result);

	double dresult = mClass().eval<double>(&sample, "date.place.x");
	double dexp = sample.getByValProp().place.x;
	EXPECT_EQ(dexp, dresult);
}

TEST_F(MetaTypeTest, NestedByPtrMutator) {
	Point * p = new Point();
	p->x = -1;
	p->y = -1;
	sample.setByPtrProp(p);

	mClass().apply(&sample, "point.x", 47.0 );

	EXPECT_EQ(47,p->x);
}

TEST_F(MetaTypeTest, NestedByValMutator) {
	Date d;
	d.d = 1;
	d.m = 1;
	d.y = 1;
	sample.setByValProp(d);
	mClass().apply(&sample, "date.y", 2089);
	EXPECT_EQ(2089, sample.getByValProp().y);

	mClass().apply(&sample, "date.place.x", 3.0);
	EXPECT_EQ(3.0, sample.getByValProp().place.x);
}

TEST_F(MetaTypeTest, testPropsRO) {

	EXPECT_TRUE(mClass()["testDouble"].isReadWrite());
	EXPECT_FALSE(mClass()["testRO"].isWritable());
	EXPECT_TRUE( jrtti::metatype<Date>().property("d").isWritable() );

	int result = (mClass()["testRO"].get<int>(&sample));
	EXPECT_EQ(23, result);
}

TEST_F(MetaTypeTest, Serialize) {
	Point * point = new Point();
	point->x = 45;
	point->y = 80;

	Date date;
	date.d = 1;
	date.m = 4;
	date.y = 2011;
	date.place.x = 98;
	date.place.y =93;
	sample.intMember = 128;
	sample.setDoubleProp(65.0);
	sample.setStdStringProp(kHelloString);
	sample.setByValProp(date);
	sample.setByPtrProp(point);
	sample.setBool( true );
	std::vector< Date >& col = sample.getCollection();
	for (int i = 0; i < 2; i++) {
		++date.y;
		col.push_back( date );
	}
	for (int i = 0; i< 5; ++i )
		sample.getArray()[i] = i+10;

	std::string ss = mClass().toStr(&sample);
	std::ofstream fs("asString");
	fs << ss;

	ss.erase( std::remove_if( ss.begin(), ss.end(), ::isspace ), ss.end() );
	std::string serialized = "{\"circularRef\":{},\"collection\":{\"properties\":{},\"elements\":[{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2012},{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2013}]},\"date\":{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2011},\"intAbstract\":34,\"intMember\":128,\"intOverloaded\":87,\"memoryDump\":\"CgsMDQ4=\",\"point\":{\"x\":45,\"y\":80},\"refToDate\":{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2011},\"testBool\":true,\"testDouble\":65,\"testRO\":23,\"testStr\":\"Hello,\\\"world\\\"!\\nThisisanewlinewithnonprintablechar\\u0011\"}";
	EXPECT_EQ(serialized, ss);

	//test for streamable
	ss = mClass().toStr( &sample, true );
	std::ofstream f("test");
	f << ss;
	ss.erase( std::remove_if( ss.begin(), ss.end(), ::isspace ), ss.end() );
	serialized =             "{\"$id\":\"0\",\"circularRef\":{\"$ref\":\"0\"},\"collection\":{\"properties\":{\"$id\":\"1\"},\"elements\":[{\"$id\":\"2\",\"d\":1,\"m\":4,\"place\":{\"$id\":\"3\",\"x\":98,\"y\":93},\"y\":2012},{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2013}]},\"date\":{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2011},\"intAbstract\":34,\"intOverloaded\":87,\"memoryDump\":\"CgsMDQ4=\",\"point\":{\"$id\":\"4\",\"x\":45,\"y\":80},\"refToDate\":{\"$id\":\"5\",\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2011},\"testBool\":true,\"testDouble\":65,\"testRO\":23,\"testStr\":\"Hello,\\\"world\\\"!\\nThisisanewlinewithnonprintablechar\\u0011\"}";
	EXPECT_EQ( serialized, ss );
	delete point;
}

TEST_F(MetaTypeTest, Deserialize) {
	std::ifstream fin("test");
	std::stringstream sss;
	sss << fin.rdbuf();
	std::string serialized = sss.str();//"{\"collection\":[{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2012},{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2013}],\"date\":{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2011},\"intAbstract\":34,\"intMember\":128,\"intOverloaded\":87,\"memoryDump\":\"CgsMDQ4=\",\"point\":{\"x\":45,\"y\":80},\"refToDate\":{\"d\":1,\"m\":4,\"place\":{\"x\":98,\"y\":93},\"y\":2011},\"testBool\":true,\"testDouble\":65,\"testRO\":23,\"testStr\":\"Hello,\\\"world\\\"!\"}";
	sample.circularRef = NULL;
	sample.getArray()[0]=0;
	mClass().fromStr( &sample, serialized );
	std::string ss = mClass().toStr(&sample, true);
	std::ofstream fout("test1");
	fout << ss;

	EXPECT_EQ(serialized, ss);
	EXPECT_TRUE( &sample == sample.circularRef );
	delete sample.getByPtrProp();
}

TEST_F(MetaTypeTest, testAnnotation) {
	jrtti::Annotations annotations = mClass()["testDouble"].annotations();

	GUIAnnotation * a = annotations.getFirst<GUIAnnotation>();

	EXPECT_EQ( "test.ico", a->icon() );
	a = mClass()["intMember"].annotations().getFirst< GUIAnnotation >();
	EXPECT_TRUE( a == NULL );            
}

TEST_F(MetaTypeTest, testMultipleAnnotation) {
	jrtti::Annotations annotations = mClass()["point"].annotations();

	std::vector< MenuAnnotation * > a = annotations.getAll<MenuAnnotation>();

	EXPECT_EQ( (size_t)3, a.size() );


	EXPECT_EQ( "Entry_2", a[1]->submenu() );
}

TEST_F(MetaTypeTest, testTypeAnnotation) {
	jrtti::Annotations annotations = mClass().annotations();

	GUIAnnotation * a = annotations.getFirst<GUIAnnotation>();

	EXPECT_EQ( "sample.ico", a->icon() );
}

TEST_F(MetaTypeTest, testMethodAnnotation) {
	jrtti::Annotations annotations = mClass().method( "testMethod" ).annotations();

	GUIAnnotation * a = annotations.getFirst<GUIAnnotation>();

	EXPECT_EQ( "method.ico", a->icon() );
}

TEST_F(MetaTypeTest, testCreate) {
	Point * p = boost::any_cast< Point * >( mClass()[ "point" ].metatype().create() );
	EXPECT_TRUE( (p->x == -1) && (p->y == -1) );
}

TEST_F(MetaTypeTest, testIntMethodCall) {
	Sample sample;

	mClass().call< void >("testMethod", &sample);
	int i = mClass().call< int >("testIntMethod", &sample);

	EXPECT_EQ(23, i);
}

TEST_F(MetaTypeTest, testSquareMethodCall) {
	Sample sample;
	double result = mClass().call< double >("testSquare", &sample, 4.0);
	EXPECT_EQ(16.0, result );
}

TEST_F(MetaTypeTest, testSumMethodCall) {
	Sample sample;
	double result = mClass().call<double>("testSum",&sample,9,6.0);
	EXPECT_EQ(15.0, result);
}

TEST_F(MetaTypeTest, base64) {
	const int length = 0xffff;
	uint8_t * p = new uint8_t[length];

	srand ( (unsigned int) time(NULL) );
	for (int i = 0; i < length; ++i) {
		p[ i ] = rand() % 0xff;
	}

	std::string encoded = jrtti::Base64::encode( p, length );
	uint8_t * decoded = jrtti::Base64::decode( encoded );
	int i = memcmp( p, decoded, length );

	EXPECT_FALSE(i);
	delete p;
	delete decoded;
}

TEST_F(MetaTypeTest, testCollectionInterface) {
	MyCollection col;
	col.intMember = 153;

	jrtti::declareCollection< MyCollection >()
		.property( "intMember", &MyCollection::intMember );

	MyCollection::iterator it = col.begin();

	for ( int i = 0; i<20; ++i) {
		it = col.insert( it, i );
		++it;
	}

	std::string res = jrtti::metatype< MyCollection >().toStr( &col );

	col.intMember = 0;
	jrtti::metatype< MyCollection >().fromStr( &col, res );

	EXPECT_EQ( res.length(), jrtti::metatype< MyCollection >().toStr( &col ).length() );
	EXPECT_EQ( col.intMember, 153 ); 
}

TEST_F(MetaTypeTest, testMetaobject) {
	Sample sample;
	Metaobject mo = Metaobject( mClass(), &sample );

	mo.set( "date.d", 34 );
	EXPECT_EQ( 34, mo.get<int>( "date.d" ) );

	mo.set( "testDouble", 34.8 );
	EXPECT_EQ( 34.8, mo.get<double>( "testDouble" ) );

	mo.set( "date.y", mo.get( "date.d" ) );
	EXPECT_EQ( sample.getByValProp().d, sample.getByValProp().y );

	SampleBase * samplePtr = mo.objectInstance< SampleBase >();
	EXPECT_TRUE( samplePtr == &sample );

	sample.intMember = 12;
	Metaobject moInt = Metaobject( mClass(), samplePtr );
	EXPECT_EQ( moInt.get<int>("intMember"), 12 );

//	std::cout << mo.toStr() << std::endl;
}

TEST_F(MetaTypeTest, comparationOperators) {
	Metatype &mt_sample = jrtti::metatype<Sample>();
	Metatype &mt_date = jrtti::metatype<Date>();
	Metatype &mt_point = jrtti::metatype<Point>();

	EXPECT_TRUE( ( mt_date == mt_sample["date"].metatype() ) );
	EXPECT_TRUE( ( mt_date == mt_sample["refToDate"].metatype() ) );
	EXPECT_TRUE( ( mt_point != mt_sample["point"].metatype() ) );
}

TEST_F(MetaTypeTest, parentCheck) {
	Metatype &mt_sample = jrtti::metatype<Sample>();
	Metatype &mt_date = jrtti::metatype<Date>();

	EXPECT_FALSE( mt_date.isDerivedFrom( mt_sample ) );
	EXPECT_TRUE( jrtti::metatype< SampleDerived >().isDerivedFrom( mt_sample ) );
	EXPECT_TRUE( jrtti::metatype< SampleDerived >().isDerivedFrom< SampleBase >() );

	Metatype &mtp_sample = jrtti::metatype<Sample *>();
	Metatype &mtp_date = jrtti::metatype<Date*>();

	EXPECT_FALSE( mtp_date.isDerivedFrom( mtp_sample ) );
	EXPECT_TRUE( jrtti::metatype< SampleDerived * >().isDerivedFrom( mtp_sample ) );
	EXPECT_TRUE( jrtti::metatype< SampleDerived *>().isDerivedFrom< SampleBase * >() );
}

TEST_F(MetaTypeTest, queryTypeAttributes) {
// isAbstract
	EXPECT_FALSE( jrtti::metatype<Date>().isAbstract() );
	EXPECT_TRUE( jrtti::metatype<SampleBase>().isAbstract() );

	EXPECT_FALSE( jrtti::metatype<Date *>().isAbstract() );
	EXPECT_TRUE( jrtti::metatype<SampleBase *>().isAbstract() );

// isCollection
	jrtti::declareCollection< MyCollection >()
		.property( "intMember", &MyCollection::intMember );

	EXPECT_FALSE( jrtti::metatype<Sample>().isCollection() );
	EXPECT_TRUE( jrtti::metatype< MyCollection >().isCollection() );

	EXPECT_FALSE( jrtti::metatype<Sample *>().isCollection() );
	EXPECT_TRUE( jrtti::metatype< MyCollection *>().isCollection() );

}

struct TestUntyped {
	int i;
	void * ptr;
};

TEST_F(MetaTypeTest, untypedProperty) {
	TestUntyped testUntyped;

	Metatype& mt = declare< TestUntyped >();
	UntypedProperty< TestUntyped > * prop = new UntypedProperty< TestUntyped >( metatype< Point * >(), "untyped" );
	prop->member( &TestUntyped::ptr );
	mt.addProperty( "untyped", prop ); 

	Point p;
	p.x = 2;
	p.y = 3;

	mt[ "untyped" ].set( &testUntyped, &p );

	std::string s = mt.toStr( &testUntyped );
	s.erase( std::remove_if( s.begin(), s.end(), ::isspace ), s.end() );
	EXPECT_EQ( s, "{\"untyped\":{\"x\":2,\"y\":3}}" );

	s = mt[ "untyped" ].metatype().toStr( testUntyped.ptr ); 
	s.erase( std::remove_if( s.begin(), s.end(), ::isspace ), s.end() );
	EXPECT_EQ( s, "{\"x\":2,\"y\":3}" );
	
	Point p1 = jrtti_cast< Point >( mt[ "untyped" ].get( &testUntyped ) );

	Sample sample;
	mClass()[ "point" ].set( &sample, mt[ "untyped" ].get( &testUntyped ) );
	EXPECT_EQ( sample.getByPtrProp()->y, 3 );

	mClass().apply( &sample, "date.place", mt[ "untyped" ].get( &testUntyped ) );
	EXPECT_EQ( sample.getByRefProp().place.y, 3 );
}

TEST_F(MetaTypeTest, checkUseCase) {
	useCase();
}

GTEST_API_ int main(int argc, char **argv) {
	std::cout << "Running tests\n";

	testing::InitGoogleTest(&argc, argv);
	int test_result = RUN_ALL_TESTS();
	getc(stdin);
	return test_result;
}


