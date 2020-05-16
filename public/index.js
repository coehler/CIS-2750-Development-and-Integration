// Put all onload AJAX calls here, and event listeners
var host = '';
$(document).ready(function() {
	
	/* - - - - - - - -*/
	/* INIT FUNCTIONS */
	/* - - - - - - - -*/
	
	document.getElementById('buildDB').disabled = true; 
	document.getElementById('clearDB').disabled = true; 
	document.getElementById('displayDB').disabled = true; 
	
	document.getElementById('q1sub').disabled = true; 
	document.getElementById('q2sub').disabled = true; 
	document.getElementById('q3sub').disabled = true; 
	document.getElementById('q4sub').disabled = true; 
	document.getElementById('q5sub').disabled = true; 
	document.getElementById('q6sub').disabled = true; 

	
	/* This populates the GEDCOM file list */
	$.ajax({
        type: 'get',            
        dataType: 'text',       
        url: '/getAll',   
        success: function (data) {
			
			var files = [];
			files = data.split(",");
			var i;
			if(files == ''){
				return;
			}
			for( i = 0; i < files.length; i ++ ){
				
				parseGED(files[ i ]);
				/* Here we populate our drop down menus */
				$('#showsel').append($('<option/>', { 
					text : files[ i ]
				}));
				$('#q3').append($('<option/>', { 
					text : files[ i ]
				}));
				
			}
			
			if(files.length > 0){
				parseFile( files[0] );
			}
			
            
        },
        fail: function(error) {
            console.log(error); 
        }
    });
    

	
    /* - - - - - - - - - */
    /* FILE MANIPULATION */
    /* - - - - - - - - - */
    
    
	$('#createFile').click(function(){
		
		var filename='';
		var subname='';
		var subaddr='';
		var encoding ='';
		
		var filename = $('#filename').val() + ".ged";
		var subname = $('#subname').val();
		var subaddr = $('#subaddr').val();
		var encoding = $('#showencode').find(":selected").text();
		
		if( filename == '' || subname == '' ||  subaddr == '' || encoding == ''){
			writeStatusMessage ("Cannot Create File. One or More Fields Are Empty.", true);
			return;
		}
				
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/createFile',
			data: {
				filename: filename,
				subname: subname,
				subaddr: subaddr,
				encoding: encoding
			},   

			success: function (data) {
				/* Parse and Display JSON here */
				writeStatusMessage (data, false);
				parseGED(filename);
				$('#showsel').append($('<option/>', { 
					text : filename
				}));
				$('#q3').append($('<option/>', { 
					text : filename
				}));
				
			},
			fail: function(error) {
				alert(error); 
			}
		});
		
	});
	
	$('#sendInd').click(function(){
		
		var firstname='';
		var lastname='';
		
		
		var firstname = $('#indgivn').val();
		var lastname = $('#indsurn').val();
		var filename = $('#showsel').find(":selected").text();
		
		if( firstname == '' || subname == ''){
			writeStatusMessage ("Cannot Add Person. One or More Fields Are Empty.", true);
			return;
		}
				
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/addInd',
			data: {
				filename: filename,
				firstname: firstname,
				lastname: lastname,
			},   

			success: function (data) {
				/* Parse and Display JSON here */
				writeStatusMessage (data, false);
				clearIndList();
				parseFile(filename);
			},
			fail: function(error) {
				alert(error); 
			}
		});
		
	});
	
	
    /* This function uploads a file */
    $('#submitFile').click(function(){

		var uploadFile = document.getElementById("uploadFile");
		var uploadButton = document.getElementById("submitFile");
		
		/* Has to be in a form object. Create empty one here. */
		var formData = new FormData();
		
		/* Get file name */
		var fileObject = uploadFile.files[0];
		
		if (fileObject == undefined) {

			writeStatusMessage ("No file selected", true);

		} else {

			if(isGEDCOM(fileObject.name)){

				writeStatusMessage ("Uploading file " + fileObject.name, false);
				
				/* Add our components to the form */
				formData.append("fileSelect", fileObject);
				
				/* Set end-point */
				var url = "/upload";
				
				/* Create request object */
				var request = new XMLHttpRequest();

				/* For every state change request goes through, execute */
				request.onreadystatechange = function() {
					
					/* When request ready, print to webpage */
					if (request.readyState == XMLHttpRequest.DONE) {
						writeStatusMessage ("Server returned " + request.statusText +" (" + request.status.toString() + ").", request.status!=200 );
						
						/* Add file to File Log table */
						parseGED(fileObject.name);
						
						/* We need to update the drop down list */
						$('#showsel').append($('<option/>', { 
							text : fileObject.name
						}));
						
						$('#q3').append($('<option/>', { 
							text : fileObject.name
						}));
					}
				};
				
				/* Open post request with end-point */
				request.open("POST", url, true);
				request.send(formData);
				
			} else {
				writeStatusMessage ("Selected file is not a GEDCOM file.", true);
			}
		}
        
    });
	
	/* This function parses a GEDCOM's file and adds it to the individual listing */
    function parseFile(filename){

        $.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/populateInd',
			data: {
				file: filename
			},   

			success: function (data) {
				
				var JSONindList = JSON.parse(data);
				
				var givn;
				var surn;
				var full;
				var row;
				
				for( var i = 0; i < JSONindList.length; i ++ ){
					givn = JSONindList[i].givenName;
					surn = JSONindList[i].surname;
					full = givn + " " + surn
					
					$('#gedcomtable').append('<tr class="child"><td>' + givn + '</td><td>' + surn + '</td><td>M/F</td><td>Unknown</td></tr>');
					
					
					$('#showdes').append($('<option/>', { 
						text : full
					}));
					
					$('#showan').append($('<option/>', { 
						text : full
					}));
					
					$('#q4').append($('<option/>', { 
						text : full
					}));
					
					$('#q6').append($('<option/>', { 
						text : full
					}));
				}
				$('#statustable').append('<tr class="child"><td>Displaying ' + filename + '</td></tr>');
			},
			fail: function(error) {
				alert(error); 
			}
		});
		
    }
    
    /* This function parses a GEDCOM's header and adds it to the file listing */
    function parseGED(filename){

        $.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/populateGed',
			data: {
				file: filename
			},   

			success: function (data) {
				
				var JSONindList = JSON.parse(data);

				var source = JSONindList.source;
				var gedc = JSONindList.GEDCOMversion;
				var encoding = JSONindList.encoding;
				var subname = JSONindList.subname;
				var numFam = JSONindList.numFam;
				var numInd = JSONindList.numInd;

				$('#filelogtable').append('<tr class="child"><td><a href="/uploads/' + filename + '">' + filename + '</a></td><td>' + source + '</td><td>' + gedc + '</td><td>' + encoding +'</td><td>' + subname + '</td><td>N/A</td><td>' + numInd + '</td><td>' + numFam + '</td></tr>');
		
			},
			fail: function(error) {
				alert(error); 
			}
		});
	
    }
    
    /* - - - - - - - - - - - */
    /* DISPLAY MANIUPULATION */
	/* - - - - - - - - - - - */
	
	$('#subfilesel').click(function(){
		
		var file = $('#showsel').find(":selected").text();
		
		clearIndList();
		
		$('#showdes').empty();
		$('#showan').empty();
				
		parseFile(file);
		
    });
    
    /* - - - - - - - - - - -*/
    /* GENERATION FUNCTIONS */
	/* - - - - - - - - - - -*/
	
	$('#desSend').click(function(){
		
		var sel = $('#showdes').find(":selected").text();
		var num = $('#numdes').val();
		var file = $('#showsel').find(":selected").text();
		
		var numShow;
		if( num == 0 ){
			numShow = "All";
		}else{
			numShow = num;
		}
		var msg = ("Displaying " + numShow + " Generations of " + sel + "'s Descendants");
		writeStatusMessage (msg, false);
		
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/getDes',
			data: {
				filename: file,
				indName: sel,
				numGen: num
			},   

			success: function (data) {
				/* Parse and Display JSON here */
				
				if(data == "[]"){
					clearGenList();
					writeStatusMessage ("No Descendants of " + sel + " found", false);
				}
				
				var toAppend = "<tr>";
				var JSONgenList = JSON.parse(data);
				var name;
				
				clearGenList();
				
				for( var i = 0; i < JSONgenList.length; i ++ ){

					toAppend = "<tr><td><b>Generation " + (i + 1) + '</b></td>';
					
					for( var j = 0; j < JSONgenList[i].length; j ++ ){
						name = JSONgenList[i][j].givenName + " " + JSONgenList[i][j].surname;
						toAppend+="<td>" + name + "</td>"
					}
					
					toAppend += "</tr>"
					
					$('#gedcomtablegen').append( toAppend );
					
				}
				
			},
			fail: function(error) {
				alert(error); 
			}
		});
		
    });
    
    $('#anSend').click(function(){
		
		var sel = $('#showan').find(":selected").text();
		var num = $('#numan').val();
		var file = $('#showsel').find(":selected").text();
		
		var numShow;
		if( num == 0 ){
			numShow = "All";
		}else{
			numShow = num;
		}
		var msg = ("Displaying " + numShow + " Generations of " + sel + "'s Ancestors");
		writeStatusMessage (msg, false);
		
		
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/getAn',
			data: {
				filename: file,
				indName: sel,
				numGen: num
			},   

			success: function (data) {
				/* Parse and Display JSON here */
				
				if(data == "[]"){
					clearGenList();
					writeStatusMessage ("No Ancestors of " + sel + " found", false);
				}
				
				var toAppend = "<tr>";
				var JSONgenList = JSON.parse(data);
				var name;
				
				clearGenList();
				
				for( var i = 0; i < JSONgenList.length; i ++ ){

					toAppend = "<tr><td><b>Generation " + (i + 1) + '</b></td>';
					
					for( var j = 0; j < JSONgenList[i].length; j ++ ){
						name = JSONgenList[i][j].givenName + " " + JSONgenList[i][j].surname;
						toAppend+="<td>" + name + "</td>"
					}
					
					toAppend += "</tr>"
					
					$('#gedcomtablegen').append( toAppend );
					
				}
				
			},
			fail: function(error) {
				alert(error); 
			}
		});
		
    });
    
	/* - - - - - - - - - - -*/
    /* MY SQL MANIUPULATION */
	/* - - - - - - - - - - -*/
	
	$('#connectDB').click(function(){
		var n = $('#uname').val();
		var p = $('#pass').val();
		var d = $('#dbname').val();
		var h = 'dursley.socs.uoguelph.ca';
		
		/* Send request and set back-end creds. If tables do not exist, make them */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/connectDB',
			data: {
				host: h,
				user: n,
				pass: p,
				db: d
			},   

			success: function (data) {
				if(data == 'Connection Not Made'){
					writeStatusMessage ("Connection Failure", true);
					document.getElementById('buildDB').disabled = true; 
					document.getElementById('clearDB').disabled =  true; 
					document.getElementById('displayDB').disabled =  true; 
					document.getElementById('q1sub').disabled =  true; 
					document.getElementById('q2sub').disabled =  true; 
					document.getElementById('q3sub').disabled =  true; 
					document.getElementById('q4sub').disabled =  true; 
					document.getElementById('q5sub').disabled =  true; 
					document.getElementById('q6sub').disabled =  true;
				}else{
					
					
					
					msg = ("Connection Success");
					writeStatusMessage (msg, false);
					
					
					
					document.getElementById('buildDB').disabled = false; 
					document.getElementById('clearDB').disabled = false; 
					document.getElementById('displayDB').disabled = false; 
					document.getElementById('q1sub').disabled = false; 
					document.getElementById('q2sub').disabled = false; 
					document.getElementById('q3sub').disabled = false; 
					document.getElementById('q4sub').disabled = false; 
					document.getElementById('q5sub').disabled = false; 
					document.getElementById('q6sub').disabled = false;
					displayDB();
				}
			},
			fail: function(error) {
				alert(error); 
			}
		});
		
    });
	
	/* For every file we make an AJAX to get file data */
	function sqlFromFile( filename ) {
		
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/populateGed',
			data: {
				file: filename
			},   

			success: function (data) {
				
							
				/* Now that file data is secured, we format the data into a SQL request */
				var JSONindList = JSON.parse(data);

				var source = JSONindList.source;
				var gedc = JSONindList.GEDCOMversion;
				var encoding = JSONindList.encoding;
				var subname = JSONindList.subname;
				var numFam = JSONindList.numFam;
				var numInd = JSONindList.numInd;
							
				insert = ("INSERT INTO FILE" + 
				
					"(file_Name," +
					"source," + 
					"version," +
					"encoding," + 
					"sub_name,"+
					"num_individials," +
					"num_families) " + 
				
				"VALUES (" + "'" + 
				
					filename + "'," + "'" + 
					source + "'," + "'" + 
					gedc + "'," + "'" + 
					encoding + "'," + "'" + 
					subname + "'," + 
					numInd + "," + "" + 
					numFam + ")");
				
				console.log(insert);
				
				/* Now that we have a SQL statment, make another AJAX call to put it into the server */
				
				$.ajax({
					type: 'get',            	
					dataType: 'text',   	
					url: '/insertFileDB',
					data: {
						q: insert
					},   

					success: function (data) {
						
						/* Get file_id from DB via Ajax call */
						$.ajax({
							type: 'get',            	
							dataType: 'text',   	
							url: '/getFileID',
							data: {
								filename: filename
							},   

							success: function (data) {
								
								/* File id is gotten. */
								
								
								
							},
							fail: function(error) {
								alert(error); 
							}
						});
						
					},
					fail: function(error) {
						alert(error); 
					}
				});
			},
				fail: function(error) {
				alert(error); 
			}

		});			
	}
	
	/* For every file we make an AJAX to get ind data */
	function sqlFromInd( filename ) {
		
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/populateInd',
			data: {
				file: filename
			},   

			success: function (data) {
				
							
				/* Now that file data is secured, we format the data into a SQL request */
				var JSONindList = JSON.parse(data);
				var givn;
				var surn;

				for( var i = 0; i < JSONindList.length; i ++ ){
					givn = JSONindList[i].givenName;
					surn = JSONindList[i].surname;					
					insert = ("INSERT INTO INDIVIDUAL" + 
					
						"(surname," +
						"given_name," + 
						"source_file) " +
						
					"VALUES (" + "'" + 
						
						surn + "'," + "'" + 
						givn + "'," + "'" + 
						filename + "')"); /* Not file name, file ID\*/
					/* Now that we have a SQL statment, make another AJAX call to put it into the server */
					console.log(insert);
					
					/* START AJAX */
						$.ajax({
							type: 'get',            	
							dataType: 'text',   	
							url: '/insertFileDB',
							data: {
								q: insert
							},   

							success: function (data) {
								
								msg = (data);
								writeStatusMessage (msg, false);
								displayDB();
								
							},
							fail: function(error) {
								alert(error); 
							}
						});
					/* END AJAX */					
					
				}

			},
				fail: function(error) {
				alert(error); 
			}

		});			
	}
	
	
	/* This function populates the FILE and INDIVIDUAL databases */
	$('#buildDB').click(function(){
		
		var files = [];
		
		$.ajax({
			type: 'get',            
			dataType: 'text',       
			url: '/getAll',   
			success: function (data) {
				
				/* Files is a list of plaintext files */
				files = data.split(",");
				if(files == ''){
					return;
				}
				
				/* Make an AJAX call to clear the TABLES */
				$.ajax({
					type: 'get',            
					dataType: 'text',       
					url: '/clearDB',   
					success: function (data) {
						
						/* At this point we are SURE the TABLES are empty and that files is populated 
						 * Proceed to generate an INSERT for each file from file name
						 * */
						 
						for( var i = 0; i < files.length; i ++ ){
							sqlFromFile( files[i] );
							//sqlFromInd( files[i] );
						}
						displayDB();
					},
					fail: function(error) {
						console.log(error); 
					}
				});
			},
			fail: function(error) {
				console.log(error); 
			}
		});
		
		
		
    });
	
	/* Q1 is some generic query starting with 'SELECT' */
	$('#q1sub').click(function(){
		var sqlQuery = "SELECT ";
		sqlQuery += $('#q1').val();
		displayDB();
		/* Query is created. Send to DB */
		
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/sqlQuery',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				
				clearDbList();
				loadTableSQL( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});
	
	/* Q2 is some generic query starting with 'SELECT' */
	$('#q2sub').click(function(){
		var sqlQuery = "SELECT * FROM INDIVIDUAL ORDER BY surname";
		displayDB();
		/* Query is created. Send to DB */
		
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/sqlQuery',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				
				clearDbList();
				loadTableSQL( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});
	
	/* Q3 is some query of ind from file */
	$('#q3sub').click(function(){
		var filename = $('#q3').find(":selected").text();
		var sqlQuery = "SELECT * FROM FILE left join INDIVIDUAL on FILE.file_id = INDIVIDUAL.source_file WHERE file_Name = '" + filename + "'";
		displayDB();
		/* Query is created. Send to DB */
		
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/getIndDB',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				
				clearDbList();
				tableDelmit( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});

	$('#q4sub').click(function(){
		var fullname = $('#q4').find(":selected").text();
		fullname = fullname.split(' ');
		var surname = fullname[1];
		displayDB();
		var sqlQuery = "SELECT * FROM INDIVIDUAL WHERE surname = '" + surname + "'";
		
		/* Query is created. Send to DB */
		
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/getIndDB',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				
				clearDbList();
				tableDelmit( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});
	
	$('#q6sub').click(function(){
		var fullname = $('#q6').find(":selected").text();
		fullname = fullname.split(' ');
		var surname = fullname[0]; 
		displayDB();
		var sqlQuery = "SELECT * FROM INDIVIDUAL WHERE given_name = '" + surname + "'";
		console.log(sqlQuery);
		/* Query is created. Send to DB */
		
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/getIndDB',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				
				clearDbList();
				tableDelmit( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});
	
	$('#q5sub').click(function(){
		var encoding = $('#q5').find(":selected").text();
		displayDB();
		var sqlQuery = "SELECT * FROM FILE WHERE encoding = '" + encoding + "'";
		
		/* Query is created. Send to DB */
		console.log(sqlQuery);
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/sqlQuery',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				
				clearDbList();
				loadTableSQL( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});
		
	/* Q1 is some generic query starting with 'SELECT' */
	$('#dbhelp').click(function(){
		var sqlQuery = "DESCRIBE FILE";
		
		displayDB();
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/sqlQuery',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				$('#dbtable').append('<tr class="child"><td>FILE</td></tr>');
				loadTableSQL( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */
		sqlQuery = "DESCRIBE INDIVIDUAL";
		
		
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/sqlQuery',
			data: {
				q: sqlQuery 
			},   

			success: function (data) {
				$('#dbtable').append('<tr class="child"><td>INDIVIDUAL</td></tr>');
				loadTableSQL( data );
								
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */			
		
	});
	
	/* Display the size of each table */
	$('#displayDB').click(function(){
		displayDB();
	});
	
	function displayDB(){
		var arr= [];
		var toPrint = '';
		/* START AJAX */
		$.ajax({
			type: 'get',            	
			dataType: 'text',   	
			url: '/displayDB',

			success: function (data) {
				arr = data.split('!');
				toPrint = "Database has ";
				toPrint += arr[0];
				toPrint += " files and ";
				toPrint += arr[1];
				toPrint += "individuals";
				writeStatusMessage (toPrint, false);
			},
			fail: function(error) {
					alert(error); 
			}
		});
		/* END AJAX */	
		
	}
	
	function tableDelmit( str ){
		var tapp = '<tr class="child">'
		
		var arr = str.split('!');
		
		for( var i = 0; i < arr.length - 1; i ++ ){
			tapp = '<tr class="child">';
			//console.log( "LINE BEFORE SPLIT ACROSS ," + arr[i]);
			arr[i] = arr[i].split(',');
			
			for( var j = 0; j < arr[i].length; j ++ ){
				
				tapp += "<td>" + arr[i][j] + "</td>";
				
			}
			tapp += '</tr>';
			$('#dbtable').append(tapp);
			tapp = "";
			console.log("Adding: " + tapp);
		}
		
		
	}
	
	function loadTableSQL( rows ){
		
		/* Takes a row object and loads a table with the results */
		
		/* ROWS is a text representation of the QUERY results
		 * We'll fromat this text into snippets that are inserted into a table
		 * (Yes, this IS a mess)*/
		 
		/* First split the the whole block '},' */
		
		var textrows = [];
		
		
		
		textrows = rows.split('},');
		
		/* Now each element of textrows is a single QUERY result*/
		for( var i = 0; i < textrows.length; i ++ ){
			
			/* For each element of text rows, split by  */
			textrows[i] = textrows[i].split(',');
			
		}
		
		/* We should now have an array of rows, were each row is an array of elements in a row */
		
		var tapp;
		for( var i = 0; i < textrows.length; i ++ ){
			
			tapp = '<tr class="child">'
			for( var j = 0; j < textrows[i].length; j ++ ){
				
				/* To get it looking a little less like ass, string out non-essential characters */
				textrows[i][j]=textrows[i][j].replace(/["']/g, " ");
				textrows[i][j]=textrows[i][j].replace(/{/g, "");
				textrows[i][j]=textrows[i][j].replace(/[[]/g, "");	
				textrows[i][j]=textrows[i][j].replace(/}/g, "");
				textrows[i][j]=textrows[i][j].replace(/]/g, "");
				/* For every element */
				tapp += "<td>" + textrows[i][j] + "</td>"
				

				
			}
			tapp += '</tr>';
			$('#dbtable').append(tapp);
		}
		
	}
	
	$('#clearDB').click(function(){
		$.ajax({
			type: 'get',            
			dataType: 'text',       
			url: '/clearDB',   
			success: function (data) {
				var msg = ("Table Cleared");
				writeStatusMessage (msg, false);
			},
			fail: function(error) {
				console.log(error); 
			}
		});
	});
	
    /* - - - - - - - - -*/
	/* HELPER FUNCTIONS */
	/* - - - - - - - - -*/
    
    /* Gets a file's extension */
    function getExtension(filename) {
		var parts = filename.split('.');
		return parts[parts.length - 1];
	}
	
	/* Checks a file's extension to ensure it is a .ged file */
	function isGEDCOM(filename) {
		var ext = getExtension(filename);
		switch (ext.toLowerCase()) {
			case 'ged':
		    return true;
		}
		return false;
	}
	
	/* Write to the Status Bar */
	function writeStatusMessage (text, isError){
		if( isError == true){
			$('#statustable').append('<tr class="child"><td>Error: ' + text + '</td></tr>');
		}else{
			$('#statustable').append('<tr class="child"><td>' + text + '</td></tr>');
		}
	}
	
	$('#clearlog').click(function(){
		$('#statustable tr').slice(1).remove();
    });
	
	/* Clear table */
	function clearIndList() {
		$('#gedcomtable tr').slice(1).remove();
	}
	
	function clearDbList() {
		$('#dbtable tr').slice(0).remove();
	}
	
	function clearGenList() {
		$('#gedcomtablegen tr').slice(0).remove();
	}

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#someform').submit(function(e){
        e.preventDefault();
        $.ajax({});
    });
    
});
