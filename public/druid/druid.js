'use strict';


/*
	Cycles
		Light - earth rotation	(day/night)
		Arcane - moon phases (moon)
		Seasons - sun distance (seasons)

	Environment
		Plants:
			Trees - grow slowly
			Shrubs - grow quickly
			Leafy - grow mediumly
			Vines - grow very quickly
			Flowers - grow very mediumly
		Forest:
			New growth
			Medium growth
			Old growth

-

v	First-person controls
v	Terrain of a plane
v	Skybox (or dome or whatever)
v	Height-displaced terrain - mostly random



Cycles
	Sun - directional light, and disc - both change colours (light clouds too, maybe)
	Seasons
		Clouds - fluctuations in clouds
		Temperature - fluctations in the temperature from -10c to 40c
	Moon - rotates almost as fast as the astral cycle
	Astral - the outer skybox, rotates, and returns each day (get the star chart for Argentina)
Inventory
	Bag
	Machete
	Spyglass
	Satchel
	Map
Encounters
	Event
	Choices
	Outcome
Environment
	Flora
	Fauna
	Water
	Terrain
Companions
	Actions
	Loitering
	Animals:
		Boar
		Wolf
		Elk
		Badger
		Fletchling
		Raven
		Falcon
		Shrew
Alchemy
	- requires: Inventory














   Make everything a reasonable colour so it doesn't hurt my eyes


*/


$(document).ready(function(){
	console.log("Druid system ready");

	var scene = new THREE.Scene();
	var camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 20000 );

	var renderer = new THREE.WebGLRenderer();
	renderer.setSize( window.innerWidth, window.innerHeight );
	$('#canvas').each(function(){
		this.appendChild(renderer.domElement);
	});

	addDebugDrawing( scene );
	addTerrain( scene, function(terrain) {
		addTree( scene, terrain );
	});
	addSkyBox( scene );
	addFirstPersonCameraControls( camera, function( controls ) {

		function render() {
			requestAnimationFrame( render );
			var delta = 1000 / 600;

			renderer.render( scene, camera );
			controls.update(delta);
		}

		render();
	});
});




function generateHeight( width, height ) {
	var size = width * height, data = new Uint8Array( size ),
	perlin = new ImprovedNoise(), quality = 1, z = Math.random() * 100;
	for ( var j = 0; j < 4; j ++ ) {
		for ( var i = 0; i < size; i ++ ) {
			var x = i % width, y = ~~ ( i / width );
			data[ i ] += Math.abs( perlin.noise( x / quality, y / quality, z ) * quality * 1.75 );
		}
		quality *= 5;
	}
	return data;
}

function generateTexture( data, width, height ) {
	var canvas, canvasScaled, context, image, imageData,
	level, diff, vector3, sun, shade;
	vector3 = new THREE.Vector3( 0, 0, 0 );
	sun = new THREE.Vector3( 1, 1, 1 );
	sun.normalize();
	canvas = document.createElement( 'canvas' );
	canvas.width = width;
	canvas.height = height;
	context = canvas.getContext( '2d' );
	context.fillStyle = '#000';
	context.fillRect( 0, 0, width, height );
	image = context.getImageData( 0, 0, canvas.width, canvas.height );
	imageData = image.data;
	for ( var i = 0, j = 0, l = imageData.length; i < l; i += 4, j ++ ) {
		vector3.x = data[ j - 2 ] - data[ j + 2 ];
		vector3.y = 2;
		vector3.z = data[ j - width * 2 ] - data[ j + width * 2 ];
		vector3.normalize();
		shade = vector3.dot( sun );
		imageData[ i ] = ( 96 + shade * 128 ) * ( 0.5 + data[ j ] * 0.007 );
		imageData[ i + 1 ] = ( 32 + shade * 96 ) * ( 0.5 + data[ j ] * 0.007 );
		imageData[ i + 2 ] = ( shade * 96 ) * ( 0.5 + data[ j ] * 0.007 );
	}
	context.putImageData( image, 0, 0 );
	// Scaled 4x
	canvasScaled = document.createElement( 'canvas' );
	canvasScaled.width = width * 4;
	canvasScaled.height = height * 4;
	context = canvasScaled.getContext( '2d' );
	context.scale( 4, 4 );
	context.drawImage( canvas, 0, 0 );
	image = context.getImageData( 0, 0, canvasScaled.width, canvasScaled.height );
	imageData = image.data;
	for ( var i = 0, l = imageData.length; i < l; i += 4 ) {
		var v = ~~ ( Math.random() * 5 );
		imageData[ i ] += v;
		imageData[ i + 1 ] += v;
		imageData[ i + 2 ] += v;
	}
	context.putImageData( image, 0, 0 );
	return canvasScaled;
}


function addFirstPersonCameraControls ( camera, callback ) {
	var controls = new THREE.FirstPersonControls( camera );
	camera.position.z = 5;
	camera.position.y = 2;
	controls.movementSpeed = 0.1;
	controls.lookSpeed = 0.0005;
	controls.noFly = true;
	controls.lookVertical = true;

	console.log("Added first person camera controls");
	if( callback ) {
		callback( controls );
	}
}

function addDebugDrawing ( scene, callback ) {
	var axes = new THREE.AxisHelper(100);
	scene.add( axes );

	var geometry = new THREE.BoxGeometry( 1, 1, 1 );
	var material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } );
	var cube = new THREE.Mesh( geometry, material );
	scene.add( cube );

	console.log("Added debug drawing");
	if( callback ) {
		callback();
	}
}

function addTerrain( scene, callback ){
	var worldWidth = 256, worldDepth = 256,
		worldHalfWidth = worldWidth / 2, worldHalfDepth = worldDepth / 2;

	var geometry = new THREE.PlaneBufferGeometry( 256, 256, worldWidth - 1, worldDepth - 1 );
	geometry.rotateX( - Math.PI / 2 );

	var data = generateHeight( worldWidth, worldDepth );
	console.log(data);

	var vertices = geometry.attributes.position.array;
	for ( var i = 0, j = 0, l = vertices.length; i < l; i ++, j += 3 ) {
		vertices[ j + 1 ] = data[ i ] * 0.05;
	}

	var texture = new THREE.CanvasTexture( generateTexture( data, worldWidth, worldDepth ) );
	texture.wrapS = THREE.ClampToEdgeWrapping;
	texture.wrapT = THREE.ClampToEdgeWrapping;
	var mesh = new THREE.Mesh( geometry, new THREE.MeshBasicMaterial( { map: texture } ) );
	mesh.position.y -= 1;
	scene.add( mesh );

	mesh.getHeight = function( x, y ) {
		// TODO: if the x,y vector is outside of the terrain, return false
		// if the x,y vector falls inside it, look up the data
		return 15;
	}

	console.log("Added terrain");
	if( callback ) {
		callback(mesh);
	}
}

function addSkyBox( scene, callback ){
	var imagePrefix = "build/skybox/";
	var directions  = ["xpos", "xneg", "ypos", "yneg", "zpos", "zneg"];
	var imageSuffix = ".jpg";

	var materialArray = [];
	for (var i = 0; i < 6; i++)
		materialArray.push( new THREE.MeshBasicMaterial({
			map: THREE.ImageUtils.loadTexture( imagePrefix + directions[i] + imageSuffix ),
			side: THREE.BackSide
		}));
	var skyMaterial = new THREE.MeshFaceMaterial( materialArray );
	// build the skybox Mesh
	var skyboxCube = new THREE.CubeGeometry( 10000, 10000, 10000 );
	var skyboxMesh = new THREE.Mesh( skyboxCube, skyMaterial );
	// add it to the scene
	skyboxMesh.doubleSided = true;
	scene.add(skyboxMesh);

	console.log("Added skybox to scene");
	if( callback ) {
		callback();
	}
}


function addTree( scene, terrain ) {

	// TODO: Get this from 

	scene.add(  );
}
