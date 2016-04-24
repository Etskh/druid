

// Core

#include <memory>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

class Random {
public:
	static float Float() {
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}
};


template< typename T >
class Array : public std::vector<T> {
	// empty
};


template< typename T >
class List : public std::list<T> {
	// empty
};

typedef std::string String;


struct Error {
	String	text;
	String	file;
	size_t	line;

	Error( String t )
		: text(t)
		, file("Unknown file")
		, line(0) {
		// empty
	}

	Error( String t, String f, size_t l )
		: text(t)
		, file(f)
		, line(l) {
		// empty
	}
};

class Object {
protected:
	void _addError (String text, String file, size_t line ) const {
		_errorList.push_back(Error( text, file, line));
	}

private:
	mutable List<Error> _errorList;
};
#define addError(x) _addError(x, __FILE__, __LINE__)


// Math

#include <cmath>



struct Vector3 {

	float x, y, z;

	static const Vector3 Up;

	static Vector3 random() {
		return Vector3(
			Random::Float(),
			Random::Float(),
			Random::Float()
		);
	}

	Vector3(){
		x = y = z = 0.0f;
	}
	Vector3( float _x, float _y, float _z ) {
		x = _x;
		y = _y;
		z = _z;
	}
	Vector3( const Vector3& ref) {
		x = ref.x;
		y = ref.y;
		z = ref.z;
	}

	void operator += ( const Vector3& ref ) {
		x += ref.x;
		y += ref.y;
		z += ref.z;
	}

	Vector3 operator * ( float scalar ) const {
		return Vector3( x*scalar, y*scalar, z*scalar );
	}

	Vector3 operator - ( const Vector3& ref ) const {
		return Vector3( x - ref.x, y - ref.y, z - ref.z );
	}

	Vector3 cross( const Vector3& ref ) const {
		return Vector3(
			y*ref.z - z*ref.y,
			z*ref.x - x*ref.z,
			x*ref.y - y*ref.z
		);
	}

	void normalize () {
		float len = getLength();
		x /= len;
		y /= len;
		z /= len;
	}

	float getLength() const {
		return sqrt( x*x + y*y + z*z );
	}
};

const Vector3 Vector3::Up = Vector3(0.0f, 1.0f, 0.0f);



// GFX


struct VertexPos {
	Vector3 pos;
	//Vector3 colour;
	//Vector3 uv;
};

struct Surface {
	/// Faces for the vertices - triangles ONLY
	size_t verts[3];
	//Vector3 colour;
};



// TODO: Make GFXObject also contain things like emitters for leaves, and LODS, and billboards, etc

class GFXObject : public Object {
public:
	typedef std::shared_ptr< GFXObject > Handle;

	/*static GFXObject::Handle create( GFXObject::Handle parent ) {
		GFXObject::Handle handle(new GFXObject(parent));
		return handle;
	}*/

	virtual bool	outputAsJSON( String*& output ) const = 0;

protected:
	GFXObject()
		: Object() {
		// empty
	}

private:
	List< GFXObject::Handle > _children;
};


class Mesh : public GFXObject {
	virtual bool	outputAsJSON( String*& output ) const {
		*output = "";
		return true;
	}
};
class Billboard : public GFXObject {
};
class Emitter : public GFXObject {
};

// Tree


struct TreeConfigs {
	float baseLength;		// with full energy, how long is a branch
	float widthHeightRatio; // the width to height ratio (0.2)
	float maxEnergy;		// the maximum number of divisions
	float branchEnergyRatio; // when a branch happens, how much energy goes to it
	//float lengthVariance; // how much variance does length have in a node (0=baseLength, 0.5= 0.5*baseLength -> 1.5*baseLength)
	//float branchTrunkRatio; // when a branch splits from the trunk, how much energy is sapped from the trunk to the branch (0.5 = even, 1 = all of it)
	//float skyCurve;			// the angle at which new nodes point upwards (0.0 - 0.999)
	//*childNodePrototype*	// when you can't create more nodes, what happens (generate a flower or something)
};









class Node {
public:
	typedef std::shared_ptr< Node > Handle;

	static Node::Handle create( const TreeConfigs& c, float e ) {
		auto node = Node::Handle(new Node(c, e, Vector3::Up, nullptr ));
		node->_self = node;
		return node;
	}

	static Node::Handle create( const TreeConfigs& c, float e, const Vector3& growthDir, Node::Handle parent ) {
		auto node = Node::Handle(new Node(c, e, growthDir, parent));
		node->_self = node;
		return node;
	}

	Node( const TreeConfigs& c, float e, const Vector3& growthDir, Node::Handle p )
		: _id (s_nextId++)
		, _configs(c)
		, _origin()
		, _growthVector(growthDir)
		, _width(0.0f)
		, _length(0.0f)
		, _energy(e)
		, _parent(p)
		, _self(nullptr) {

		_growthVector.normalize();

		// TODO: make the base relative to the maxEnergy
		_width = _length * _configs.widthHeightRatio;

		// If there's a parent, pull the node out in growthDir `length` units
		// from parent's origin
		if( _parent ) {
			_length = log10(e) * _configs.baseLength;
			_origin = _parent->_origin;
			_origin += growthDir * _length;
		}
	}

	void output_r() const {
		output_r(0);
	}

	void output_r( size_t indent ) const {
		const size_t indentBufferSize=512;
		char indentBuffer[indentBufferSize];
		memset(indentBuffer, 0, indentBufferSize);
		for( size_t i=0; i< indent*2; i++) {
			indentBuffer[i] = ' ';
		}

		printf("%s{\n", indentBuffer );
		printf("%s  id: %zu\n", indentBuffer, _id);
		printf("%s  origin: [%.2f, %.2f, %.2f]\n", indentBuffer, _origin.x, _origin.y, _origin.z);
		//printf("%s  width: %f\n", indentBuffer, _width);
		printf("%s  length: %f\n", indentBuffer, _length);
		printf("%s  energy: %f\n", indentBuffer, _energy);
		printf("%s  children: [", indentBuffer);
		auto child = _children.begin();
		while ( child != _children.end() ) {
			(*child)->output_r(indent+1);
			child++;
		}
		printf("%s  ]\n", indentBuffer);
		printf("%s}\n", indentBuffer);
	}

	size_t countChildren_r() const {
		size_t cnt = _children.size();
		auto c = _children.begin();
		while( c != _children.end()) {
			cnt += (*c)->countChildren_r();
			c++;
		}
		return cnt;
	}

	void generateChildren_r ( float energy ) {
		// TODO: Try a gemetric approach
		float childEnergy = energy - 1.0f;
		if( childEnergy <= 1 ) {
			return;
		}

		// Pick a growth vector for the child
		Vector3 growthVector(Vector3::Up);
		if( _parent ) {
			growthVector = _origin - _parent->_origin;
		}

		// TODO: Add some wiggle to the growth vector

		// Make the main trunk
		Node::Handle trunk( Node::create( _configs, childEnergy, growthVector, _self ));
		_children.push_back( trunk );

		{
			// Make the branch that runs off the side
			growthVector = growthVector.cross( Vector3::random() );
			Node::Handle branch( Node::create( _configs, childEnergy * _configs.branchEnergyRatio, growthVector, _self ));
			_children.push_back( branch );
			branch->generateChildren_r( branch->_energy );
		}

		// Now work our way up
		trunk->generateChildren_r( childEnergy );
	}

private:
	/// The unique Id for this Node
	size_t _id;
	/// List of configuration settings for the node
	const TreeConfigs& _configs;
	/// The 3D placement
	Vector3 _origin;
	/// Normalized vector of growth
	Vector3 _growthVector;
	/// Thickness of the node
	float _width;
	/// Length of the branch from the parent
	float _length;
	/// Energy remaining
	float _energy;
	/// List of all children
	List<Node::Handle> _children;
	/// Parent node
	Node::Handle _parent;
	/// Itself
	Node::Handle _self;

	/// Next Id for all nodes generated - these should not be used as
	/// unique identifiers amond different generated flora
	static size_t s_nextId;
};

size_t Node::s_nextId = 1;











class Tree : public GFXObject {
public:
	typedef std::shared_ptr< Tree > Handle;

	static Tree::Handle generate( int seed, const TreeConfigs& configs ) {
		Tree::Handle tree(new Tree(configs));

		tree->_rootNode = Node::create(configs, configs.maxEnergy );
		tree->_rootNode->generateChildren_r( configs.maxEnergy );

		tree->generateMesh();

		//tree->_rootNode->output_r();

		return tree;
	}

	size_t countNodes() const {
		return _rootNode->countChildren_r() + 1;
	}

	virtual bool	outputAsJSON( String*& output ) const {
		*output = "";
		return true;
	}

private:
	const TreeConfigs& _configs;
	unsigned int _nodeCount;
	Node::Handle _rootNode;

	Tree ( const TreeConfigs& configs )
		: _configs(configs) {
		// empty
	}

	void generateMesh() {
		// empty
	}
};





#include <chrono>
#include <thread>

#include <ctime>

int main( int argc, char* argv[] ) {

	int treeCount = 10000;
	int progressSkip = treeCount / 19;
	size_t waitMilliseconds = 8;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for(int t = 0; t < treeCount; t++ ) {
		TreeConfigs treeData;

		treeData.baseLength = 1.0f;
		treeData.widthHeightRatio = 0.3f;
		treeData.maxEnergy = 10.0f;
		treeData.branchEnergyRatio = 0.5f;

		auto tree = Tree::generate( 1, treeData );

		//printf("Tree generated with %zu nodes\n", tree->countNodes() );
		if( t % progressSkip == 0 ) {
			std::this_thread::sleep_for(std::chrono::milliseconds(waitMilliseconds));
			printf("%d...", t);
		}
	}
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;
	printf("\nTook %f seconds to generate %d trees\n", elapsed_seconds.count(), treeCount );

	return 0;
}

/*
## Todo

*


## Conventions

* State knows how to format itself
* Recusive functions end in `_r`
* Accessors that return state must start with `get`
* Accessors that return computed values must be named such that it isn't just
"get this", instead it should be "compute this" or "count these"
* Classes that use Handles should have static factory functions
* Classes that use Handles should have a field called `self` that the factory sets
* Classes that use Handles should have private constructors
* All functions that return more than 64 bits are deprecated
* Use doxygen to generate the documentation for the code
* If a function needs to return a large object,
	* It must the first parameter in the function
	* It must be passed by a reference to a pointer
	* It must return a bool (success is `true`)
	* The base class must be Object
	* If the function returns `false`, it must add at least one error


*/
