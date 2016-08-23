'use strict';

var express = require('express');
var fs = require('fs');

var treegen = require('./treegen/treegen');

var app = express();



app.use(express.static(__dirname + '/public'));

app.get('/', function(req, res){
  res.send(fs.readFileSync('server/views/index.html', 'utf8'));
});

app.get('/assets/tree/generate', function(req, res){
  var config = {
    baseLength: 15.0
  };
  treegen.generate(config, function( tree ){
    res.send( JSON.stringify(tree));
  });
});


app.listen(3000);
