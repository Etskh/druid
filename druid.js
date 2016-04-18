'use strict';

var express = require('express');
var app = express();

var fs = require('fs');

app.use(express.static(__dirname + '/public'));

app.get('/', function(req, res){
  res.send(fs.readFileSync('server/views/index.html', 'utf8'));
});

app.listen(3000);
