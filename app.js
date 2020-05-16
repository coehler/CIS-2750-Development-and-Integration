'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  console.log('Get index.html');
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
 //   res.send(minimizedContents._obfuscatedCode);
    res.send(contents);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

  
  if(!req.files) {
    console.log('No File');
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.fileSelect;
 
  // Use the mv() method to place the file somewhere on your server
  console.log('uploads/' + uploadFile.name);
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
	 console.log('Error movingS');	
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
  

    
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ********************


/* - - - - - - */
/* FFI Library */
/* - - - - - - */

var GEDCOMparser = ffi.Library('./GEDCOMparser.so', {
  "getIntTest": [ 'int', [ 'char' ] ],
  "JSONgetIndList": [ 'string', ['string'] ],
  "getString": [ 'string', [  ] ],
  "returnString": [ 'string', [ 'string' ] ],
  "JSONgetGEDCOM": [ 'string', [ 'string' ] ],
  "JSONgetDesList": [ 'string', [ 'string', 'string', 'int' ] ],
  "JSONgetAnList": [ 'string', [ 'string', 'string', 'int' ] ],
  "createNewFile": [ "void", ['string', 'string','string','string',]],
  "addIndFromName": [ "void", ['string', 'string','string']],
  "JSONvalidate": [ 'string', [ 'string' ] ]
  
});
/* - - - - - - - - */
/* File Validation */
/* - - - - - - - - */

app.get('/validateFile',function(req,res){
	var filename = req.query.filename;
	var results = GEDCOMparser.JSONvalidate( "./uploads/" + filename );
	res.send(results);
});

/* - - - - - - - - -*/
/* Table Population */
/* - - - - - - - - -*/

app.get('/populateInd',function(req,res){
	var filename = req.query.file;
	var results = GEDCOMparser.JSONgetIndList( "./uploads/" + filename );
	res.send(results);
});

app.get('/populateGed',function(req,res){
	var filename = req.query.file;
	var results = GEDCOMparser.JSONgetGEDCOM( "./uploads/" + filename );
	res.send(results);
});

/* - - - - - - - - - */
/* File Manipulation */
/* - - - - - - - - - */

app.get('/getAll',function(req,res){
	
	const testFolder = './uploads/';
	const fs = require('fs');
	var results = "";
	var count=0;
	
	fs.readdirSync(testFolder).forEach(file => {
		
		if(count != 0){
			results += ",";
		}
		
		results += file;
		count = count + 1;
		
	})
	
	res.send(results);
});

app.get('/createFile',function(req,res){
	
	console.log("Got to backend");
	
	var filename = req.query.filename;
	var subname = req.query.subname;
	var subaddr = req.query.subaddr;
	var encoding = req.query.encoding;
	
	console.log("BACKEND filename:" + filename);
	console.log("BACKEND subname:" + subname);
	console.log("BACKEND subaddr:" + subaddr);
	console.log("BACKEND encoding:" + encoding);
	
	GEDCOMparser.createNewFile( "./uploads/" + filename, subname, subaddr, encoding );
	
	var results = 'File ' + filename + " Created";
	res.send(results);
	
});

app.get('/addInd',function(req,res){
	
	console.log("Got to backend");
	
	var filename = req.query.filename;
	var firstname = req.query.firstname;
	var lastname = req.query.lastname;
	
	console.log("BACKEND filename:" + filename);
	console.log("BACKEND firstname:" + firstname);
	console.log("BACKEND lastname:" + lastname);
	
	GEDCOMparser.addIndFromName( "./uploads/" + filename, firstname, lastname );
	
	var results = 'Added person '+ firstname + " " + lastname + ' to ' + filename;
	res.send(results);
	
});
/* - - - - - - - - - -*/
/* Generation Getting */
/* - - - - - - - - - -*/

app.get('/getDes',function(req,res){
	
	var filename = req.query.filename;
	var name = req.query.indName;
	var number = req.query.numGen
	
	console.log("FILE:" + filename);
	console.log("NAME:" + name);
	console.log("NUMB:" + number);
	
	var results;
	results = GEDCOMparser.JSONgetDesList( "./uploads/" + filename, name, number );
	res.send(results);
});

app.get('/getAn',function(req,res){
	
	var filename = req.query.filename;
	var name = req.query.indName;
	var number = req.query.numGen
	
	console.log("FILE:" + filename);
	console.log("NAME:" + name);
	console.log("NUMB:" + number);
	
	var results;
	results = GEDCOMparser.JSONgetAnList( "./uploads/" + filename, name, number );
	res.send(results);
	
});

/* - - - - - - - - - - -*/
/* MY SQL MANIUPULATION */
/* - - - - - - - - - - -*/

const mysql = require('mysql');

var h;
var u;
var p;
var d;

app.get('/connectDB',function(req,res){
	
	var hostname = req.query.host;
	var username = req.query.user;
	var password = req.query.pass;
	var database = req.query.db;
	
	var results;
	
	console.log("HOSTNAME:" + hostname);
	console.log("USERNAME:" + username);
	console.log("PASSWORD:" + password);
	console.log("DATABASE:" + database);
	
	h = hostname;
	u = username;
	p = password;
	d = database;
	
	/* Create connection */
	const connection = mysql.createConnection({
		host: hostname,
		user: username,
		password: password,
		database: database
	});

	connection.query("SELECT 1 FROM FILE LIMIT 1;", function (err, rows, fields) {
		
		/* Error will be thrown if FILE does not exits */
		if (err){
			
			/* Create File */
			console.log("No FILE table");
			
			connection.query("create table FILE (file_id INT AUTO_INCREMENT, file_Name VARCHAR(60) NOT NULL, source VARCHAR(250) NOT NULL, version VARCHAR(10) NOT NULL, encoding VARCHAR(10) NOT NULL, sub_name VARCHAR(62) NOT NULL, sub_addr VARCHAR(256), num_individials INT, num_families INT, primary key(file_id) )", function (err, rows, fields) {
				if (err) console.log("Something went wrong. "+err);
			});
			
		};
		
		/* ONCE THE FILE TABLE IS BUILT OR CONFIRMED, CHECK FOR NEXT */
		/* QUERY START */
		connection.query("SELECT 1 FROM INDIVIDUAL LIMIT 1;", function (err, rows, fields) {
			
			/* Error will be thrown if INDIVIDUAL does not exits */
			if (err){
				
				/* Create File */
				console.log("No INDIVIDUAL table");
				
				connection.query("create table INDIVIDUAL (ind_id INT AUTO_INCREMENT, surname VARCHAR(256) NOT NULL, given_name VARCHAR(256) NOT NULL, sex VARCHAR(1), fam_size INT, source_file INT, primary key(ind_id), FOREIGN KEY(source_file) REFERENCES FILE(file_id) ON DELETE CASCADE)", function (err, rows, fields) {
					if (err) console.log("Something went wrong. "+err);
				});
				
			};
			
			connection.query("SELECT * FROM FILE;", function (err, rows, fields) {
				
				if (err){
					results = ("Connection Not Made");
					console.log("can't handle SELECT * FROM FILE;");
					connection.end();
					res.send(results);
				}else{
					console.log("handled SELECT * FROM FILE;");
					results = ("Connection Made");
					connection.end();
					res.send(results);
				}
				
			});
			
			
		});
		/* QUERY END */
		
	});
	

	

	
});

app.get('/clearDB',function(req,res){
	
	/* Create connection */
	const connection = mysql.createConnection({
		host: h,
		user: u,
		password: p,
		database: d
	});
	console.log("DELETE FROM FILE WHERE 1 = 1;");
	connection.query("DELETE FROM FILE WHERE 1 = 1", function (err, rows, fields) {
		
		/* Error will be thrown if FILE does not exits */
		if (err){
			results = ("No Connection Made");
		};
		
		connection.end();
		
	});
	
	var results;
	results = ("Connection Made");
	
	res.send(results);
	
});

app.get('/sqlQuery',function(req,res){
	
	var results;
	var str;
	
	/* Create connection */
	const connection = mysql.createConnection({
		host: h,
		user: u,
		password: p,
		database: d
	});
	
	console.log(req.query.q);
	connection.query(req.query.q, function (err, rows, fields) {
		
		/* Error will be thrown if FILE does not exits */
		if (err){
			console.log("Error" + err);
			results = ("No Connection Made");
		}else{
			console.log("No error");
			/* Extract results of an arbitary query */
			
			results = (rows);	
			res.send(rows);
	
			
		};
		
		connection.end();
		
	});
	
});

app.get('/insertFileDB',function(req,res){
	
	/* Create connection */
	const connection = mysql.createConnection({
		host: h,
		user: u,
		password: p,
		database: d
	});
	console.log("QUERY:" + req.query.q);
	connection.query(req.query.q, function (err, rows, fields) {
		
		/* Error will be thrown if FILE does not exits */
		if (err){
			results = ("No Connection Made");
		};
		
		connection.end();
		
	});
	
	var results;
	results = ("Connection Made");
	res.send(results);
	
});

app.get('/getIndDB',function(req,res){
	var format = "";
	/* Create connection */
	const connection = mysql.createConnection({
		host: h,
		user: u,
		password: p,
		database: d
	});
	console.log("QUERY:" + req.query.q);
	connection.query(req.query.q, function (err, rows, fields) {
		
		/* Error will be thrown if FILE does not exits */
		if (err){
			results = ("No Connection Made");
		}else{
			for (let row of rows){
				format += 'ind_id: ';
				format += row.ind_id + ',';
				format += 'source_file: ';
				format += row.source_file + ',';
				format += row.surname + ',';
				format += row.given_name;
				format += '!';
				console.log( format );
			}
		}
		
		var results;
		results = format;
		res.send(results);
		connection.end();
		
	});
	

	
});

app.get('/displayDB',function(req,res){
	
	var results;
	/* Create connection */
	const connection = mysql.createConnection({
		host: h,
		user: u,
		password: p,
		database: d
	});
	var format = "";

	/* First count FILE */
	connection.query('SELECT COUNT(*) AS elem FROM FILE', function (err, rows, fields) {

		if (err){
			results = ("No Connection Made");
		}else{
			
			for (let row of rows){
				console.log(row.elem);
				format += row.elem;
				format += '!';
			}
			
			
			
			/* Now do the same for INDIVIDUAL */
			connection.query('SELECT COUNT(*) AS elem FROM INDIVIDUAL', function (err, rows, fields) {
				
				/* Error will be thrown if FILE does not exits */
				if (err){
					results = ("No Connection Made");
				}else{
					for (let row of rows){
						console.log(row.elem);
						format += row.elem;
					}
				}
				
				console.log(format);
				var results = format;
				res.send(results);
							

			});
		}

		
	});
	


});

app.get('/getFileID',function(req,res){
	var insert;
	var results = "OK";
	
	/* Create connection */
	const connection = mysql.createConnection({
		host: h,
		user: u,
		password: p,
		database: d
	});
	var sqlQuery = "SELECT file_id FROM FILE WHERE file_Name = '" + req.query.filename + "'";
	console.log("QUERY:" + sqlQuery);
	connection.query(sqlQuery, function (err, rows, fields) {
		
		/* Error will be thrown if FILE does not exits */
		if (err){
			results = ("No Connection Made");
		}else{
			
			/* Make individuals from filename and source_id */
			var cPlainText = GEDCOMparser.JSONgetIndList( "./uploads/" + req.query.filename );
			var JSONindList = JSON.parse(cPlainText);
			var givn;
			var surn;
			var id = rows[ 0 ].file_id;
			for( var i = 0; i < JSONindList.length; i ++ ){
				givn = JSONindList[i].givenName;
				surn = JSONindList[i].surname;					
				insert = ("INSERT INTO INDIVIDUAL" + 
					
					"(surname," +
					"given_name," + 
					"source_file) " +
						
				"VALUES (" + "'" + 
						
					surn + "'," + "'" + 
					givn + "'," + "" + 
					id + ")");
					 
				connection.query(insert, function (err, rows, fields) {
		
					/* Error will be thrown if FILE does not exits */
					if (err){
						results = ("No Connection Made");
					};
						
				});
							
			}
			
		}
		
		connection.end();
		res.send(results);
	});
	
});



app.get('/someendpoint', function(req , res){
  res.send({
    foo: "bar"
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
