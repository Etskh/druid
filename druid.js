'use strict'

//var express = require('express')
//const fs = require('fs')

const treegen = require('druid-treegen')

const config = {
  baseLength: 15.0,
  widthHeightRatio: 0.3,
  maxEnergy: 10.0,
  branchEnergyRatio: 0.5,
}


treegen.generate(config, function( tree ){
  console.warn(JSON.stringify(tree))
})

/*var app = express();

app.use(express.static(__dirname + '/public'));

app.get('/', function(req, res){
	res.send(fs.readFileSync('server/views/index.html', 'utf8'));
});

app.get('/assets/tree/generate', function(req, res){
	var config = {
		baseLength: 15.0,
		widthHeightRatio: 0.3,
		maxEnergy: 10.0,
		branchEnergyRatio: 0.5,
	};
	treegen.generate(config, function( tree ){
		res.send( JSON.stringify(tree));
	});
});


app.listen(3000);
*/
