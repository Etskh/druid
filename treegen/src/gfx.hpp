#ifndef _INCLUDED_GFX_HPP
#define _INCLUDED_GFX_HPP

#include "core.hpp"
#include "math.hpp"

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

#endif // _INCLUDED_GFX_HPP
