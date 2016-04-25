'use strict';

var tree = require("./build/Release/tree");

module.exports.generate = function( cb ) {
  cb( tree.generate());
};
