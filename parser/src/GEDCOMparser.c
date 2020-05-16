/**
 * GEDCOM Parsing API
 * By Christopher Oehler
 * 01/20/18
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "GEDCOMparser.h"
#include "LinkedListAPI.h"
#include "GEDCOMutilities.h"

/**
 * @brief      Parse GEDCOM format file and create object.
 *
 * @param      fileName  The file name of GEDCOM format file
 * @param      obj       The object to assign file to
 *
 * @return     { error code -- see GEDCOM errors }
 */
 
GEDCOMerror globalGEDCOMerror;

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj){

	char str[151];
	FILE* fp = fopen(fileName, "r");
	Header* GEDhead;
	GEDCOMobject* GEDobj;
	GEDobj = malloc( sizeof( GEDCOMobject ) );
    *obj = NULL;
	/**
	 * { check for INV_FILE }
	 */
	 
	
	if( fp == NULL ){
      
		return createERROR(INV_FILE, -1);

	}

	fseek(fp, 0L, SEEK_END);
	double sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	if(sz == 0){

		return createERROR(INV_FILE,-1);

	}

	int fieldCount = 0;

	/**
	 * { count the number of fields in the file }
	 */
    int i = 1;
	while( 1 ){
        // use get_line instead to handle \r, \n and \n\r
        
        if (get_line(fp, str, 150) == EOF){ break;}
        
        if (strlen(str) == 149) {
            return createERROR(INV_RECORD,i);
        }
        if (strlen(str) > 0){
            
            fieldCount++;

        }
        i++;
	}

	fseek(fp, 0, SEEK_SET);

	/**
	 * { read all fields into strings -- then, delmit these strings by whitespace and put into array of string arrays}
	 */

	char* temp;
	char* fieldStrArr;
	char** fieldElmntArr[ fieldCount ];

	for( int i = 0 ; i < fieldCount; i ++ ){
		
//		fieldStrArr = fgets(str, 150, fp);
        get_line(fp, str, 150);
        fieldStrArr = str;
        
		fieldElmntArr[ i ] = calloc( 3, sizeof(char*) );

		/**
		 * { algo to send string to arr }
		 */

		temp = strtok(fieldStrArr,"[ \t\r\n\f]");

		if ( temp != NULL){
	    	fieldElmntArr[ i ][ 0 ] = strdup2( temp );
            
            bool FlagNumeric = true;
            for (int j=0; j<strlen(fieldElmntArr[ i ][ 0 ]); j++){
                FlagNumeric = FlagNumeric && isdigit(fieldElmntArr[ i ][ 0 ][ j]);
            }
            if (FlagNumeric == false) {
                return createERROR(INV_RECORD,i+1);
            }
            
     
	    	fieldElmntArr[ i ][ 1 ] = strdup2( strtok(NULL,"[ \t\r\n\f]") );
			fieldElmntArr[ i ][ 2 ] = strdup2( strtok(NULL,"\n"));
	    	
           
            /**
             * Look for CONT record
             */
             
             if (strEqu(fieldElmntArr[ i ][ 1 ],"CONT")){
                 
                 temp = calloc(strlen(fieldElmntArr[ i-1 ][ 2 ]) + strlen(fieldElmntArr[ i ][ 2 ] + 2), sizeof(char));
                 strcpy(temp, fieldElmntArr[ i-1 ][ 2 ]);
                 strcat(temp, "\n");
                 strcat(temp, fieldElmntArr[ i ][ 2 ]);
                 
                 free(fieldElmntArr[ i-1 ][ 2 ]);
                 fieldElmntArr[ i-1 ][ 2 ] = temp;
                 free(fieldElmntArr[ i ][ 2 ]);
                 i--;
                 fieldCount--;
             }
		}
	}
		

	/**
	 * { begin parsing the fields -- prep / init vars}
	 */
	    	
	int curLvl = 0;
	int downLvl = 0;
    char padPath[64];
    char curPath[64];
    char pathArr[ fieldCount ][ 64 ];
    
    strcpy(curPath, "HEAD");
    strcpy(pathArr[0], "HEAD");
    
    List refList = initializeList( &printLink, &FreeLinkStr, &compareLink );

	/**
	 * { check if header record exists }
	 */	
     
	if ( atoi(fieldElmntArr[0][0]) == 0 && strEqu( fieldElmntArr[0][1], "HEAD" ) != true ) {
		return createERROR(INV_GEDCOM, 0);
	}

	if ( atoi(fieldElmntArr[0][0]) != 0 && strEqu( fieldElmntArr[0][1], "HEAD" ) == true ) {
		return createERROR(INV_HEADER, 0);
	}

	for( int i = 0 ; i < fieldCount; i ++ ){

		/**
		 * { if we go DOWN a level, we attach the current tag to the path }
		 */
		if( curLvl < atoi( fieldElmntArr[i][0]) ){

			if( fieldElmntArr[i][1][0] != '@' ){
                strcpy(padPath, fieldElmntArr[i][1]);
                strcat(padPath, " ");
                strncat(curPath, padPath,4);
				//strncat( curPath, strcat(fieldElmntArr[i][1], " "), 4 );
			}else{
                strcpy(padPath, fieldElmntArr[i][2]);
                strcat(padPath, " ");
                strncat(curPath, padPath,4);
//				strncat( curPath, strcat(fieldElmntArr[i][2], " "), 4 );
			}

		}
		
		/**
		 * { if we go UP a / number of level(s), we reduce the path }
		 */
		if( curLvl > atoi( fieldElmntArr[i][0]) ){

			downLvl = curLvl - atoi( fieldElmntArr[i][0] );
			curPath[strlen(curPath)-4 * (downLvl + 1)] = 0;

			if( fieldElmntArr[i][1][0] != '@' ){
                strcpy(padPath, fieldElmntArr[i][1]);
                strcat(padPath, " ");
                strncat(curPath, padPath,4);
//				strncat( curPath, strcat(fieldElmntArr[i][1], " "), 4 );
			}else{
                strcpy(padPath, fieldElmntArr[i][2]);
                strcat(padPath, " ");
                strncat(curPath, padPath,4);
//				strncat( curPath, strcat(fieldElmntArr[i][2], " "), 4 );
			}

		}

		/**
		 * { if we are at the same level, we reduce the path by one and replace the lowest order path tag }
		 */
		if( curLvl == atoi( fieldElmntArr[i][0]) ){
	
			downLvl = curLvl - atoi( fieldElmntArr[i][0] );
			curPath[strlen(curPath)-4] = 0;

			if( fieldElmntArr[i][1][0] != '@' ){
                strcpy(padPath, fieldElmntArr[i][1]);
                strcat(padPath, " ");
                strncat(curPath, padPath,4);
//				strncat( curPath, strcat(fieldElmntArr[i][1], " "), 4 );			
			}else{
                strcpy(padPath, fieldElmntArr[i][2]);
                strcat(padPath, " ");
                strncat(curPath, padPath,4);
//				strncat( curPath, strcat(fieldElmntArr[i][2], " "), 4 );			
			}

			
		}

		strcpy(pathArr[i], curPath);

		/**
		 * { get the level of the field }
		 */

		curLvl = atoi( fieldElmntArr[i][0] );
		
	}
    
//    for (int i = 0; i < fieldCount; i++){
//        printf("%s\n",pathArr[i]);
//    }
//    return createERROR(OK, -1);
	
	/* - - - - - - - - - - - - - */
   	/* - - - FIELD PARSING - - - */
    /* - - - - - - - - - - - - - */

	char* curSup;
	List indiList = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
	List famList = initializeList( &printFamily, &FreeFamily, &compareFamilies );

	/**
	 * { PARSING -- FIRST PASS }
	 * { Go through the file and build HEADer. FAMily and INDIvidual structs will be partially built, leaving out links }
	 * { A list of FAMilies and INDIvidual structs paired with references will be built for the SECOND PASS }
	 */

	Individual * ind = NULL;
	Family * fam = NULL;
	Submitter* GEDsub = NULL;
    Link* TempLink;
    
	
	for( int i = 0 ; i < fieldCount; i ++ ){

		curLvl = atoi( fieldElmntArr[ i ][ 0 ] );

		if( fieldElmntArr[i][1][0] != '@' ){

			curSup = fieldElmntArr[ i ][ 1 ];

		}else{
			
			curSup = fieldElmntArr[ i ][ 2 ];

		}

		if( strEqu(curSup, "HEAD") ){
			
			GEDhead = parseHeader( pathArr, fieldElmntArr, fieldCount, i );
            if (GEDhead == NULL) {
                return globalGEDCOMerror;
            }
        }
		else if ( i == fieldCount - 1 ) {
            
            if (strEqu(curSup, "TRLR") == false) {
                createERROR(INV_GEDCOM,-1);
                return globalGEDCOMerror;
            }
            
		}else if( strEqu(curSup, "INDI") ){
			TempLink = parseIndiLink ( pathArr, fieldElmntArr, fieldCount, i );
            if (TempLink == NULL){
                return globalGEDCOMerror;
            } else {
                insertBack( &refList, TempLink );
            }

		}else if( strEqu(curSup, "FAM") ){
            TempLink = parseFamilyLink ( pathArr, fieldElmntArr, fieldCount, i );			
            if (TempLink == NULL){
                return globalGEDCOMerror;
            } else {
                insertBack( &refList, TempLink );
            }

		}else if( strEqu(pathArr[i], "SUBM") ){
			
			GEDsub = parseSubmitter( pathArr, fieldElmntArr, fieldCount, i );
            if (GEDsub == NULL){
                return globalGEDCOMerror;
            }
		}
	}	
	GEDhead -> submitter = GEDsub;
    
	/**
	 * { PARSING -- SECOND PASS }
	 * { Go through the file and build links }
	 * { References in INDI's to FAM's and vice versa will be resolved }
	 */
	 
	Link* dummyLink;
	void *dummyVoid;

	ListIterator iter = createIterator( refList );

	void* elem;
	
	for( int i = 0 ; i < fieldCount; i ++ ){

		curLvl = atoi( fieldElmntArr[ i ][ 0 ] );

		if( fieldElmntArr[i][1][0] != '@' ){
			
			curSup = fieldElmntArr[ i ][ 1 ];

		}else{

			curSup = fieldElmntArr[ i ][ 2 ];

		}
		
		if( strEqu(curSup, "INDI") == true ){
			
			elem = nextElement(&iter);
			dummyLink = (Link*) elem;
			dummyVoid = dummyLink -> data;
			
			ind = parseIndi( pathArr, fieldElmntArr, fieldCount, i, &refList, (Individual*) dummyVoid);
			
			insertBack( &indiList, ind );
			

		}else if( strEqu(curSup, "FAM") == true ){
			
			elem = nextElement(&iter);
			dummyLink = (Link*) elem;
			dummyVoid = dummyLink -> data;
			
			fam = parseFamily( pathArr, fieldElmntArr, fieldCount, i, &refList, (Family*) dummyVoid);
			insertBack( &famList, fam );
		}

	}
	
	GEDobj -> families = famList;
	GEDobj -> individuals = indiList;
	GEDobj -> header = GEDhead;
	GEDobj -> submitter = GEDsub;

	if(GEDobj->submitter == NULL){
		return createERROR(INV_GEDCOM, -1);
	}
	
    *obj = GEDobj;
	
	clearList(&refList);
	
	return createERROR(OK, -1);
}

char* printGEDCOM(const GEDCOMobject* obj){
	
    char* TempString;
	char* tmp = malloc( sizeof(char) * 50);
	char* str = malloc( sizeof(char) * 5000 );
	strcpy( str, "" );

	if(obj == NULL){
		return str;
	}
	
	strcat( str, "[ HEADER ]\n\n" );
	
	strcat( str, "  Source: " );
	strcat(  str, obj -> header -> source );
	
	strcat( str, "\n  GEDC Version: " );
	snprintf(tmp, 50, "%f", obj->header->gedcVersion);
	strcat( str, tmp );
    free(tmp);
	
	strcat( str, "\n  Encoding: " );
	int encoding = obj->header->encoding;
	if ( encoding == ANSEL ){ 					strcat(str, "ANSEL");
	}else if( encoding == UTF8 ){ 				strcat(str, "UTF-8");
	}else if( encoding == UNICODE ){ 			strcat(str, "UNICODE");
	}else if( encoding == ASCII ){				strcat(str, "ASCII");
	}
	
	strcat( str, "\n  Other Fields: " );
	strcat( str, toString(obj->header->otherFields));
    
	strcat( str, "\n  File Submitter: " );
	strcat (str, obj->header->submitter->submitterName);

	strcat( str, "\n\n[ LIST OF INDIVIDUALS ]\n" );
    
    TempString = toString(obj->individuals);
    str = realloc(str, strlen(str) + strlen(TempString) + 100);
	strcat(str, TempString);
    free(TempString);

	strcat( str, "\n\n[ LIST OF FAMILIES ]\n" );

    TempString = toString(obj->families);
    str = realloc(str, strlen(str) + strlen(TempString) + 100);
	strcat(str, TempString);
    free(TempString);
	
	return str;
	
}

Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person){
	
	ListIterator iter = createIterator( familyRecord->individuals );
	void* elem;
	Individual* toFind;
	
	while( (elem = nextElement(&iter)) != NULL){
		
		toFind = (Individual*)elem;
		
		if( compare( toFind, person ) == true){
			return toFind;
		}

	}
	
	return NULL;
}

char* printError(GEDCOMerror err){
	
	char* tmp = malloc( sizeof(char) * 50 );
	char* str = malloc( sizeof(char) * 2048 );
	strcpy( str, "[ ERROR ]\n  Type: " );
	
	if ( err.type == OK ){ 					strcat(str, "  OK\n");
	}else if( err.type == INV_FILE ){ 			strcat(str, "  INVALID FILE\n");
	}else if( err.type == INV_GEDCOM ){ 		strcat(str, "  INVALID GEDCOM STRUCTURE\n");
	}else if( err.type == INV_HEADER ){		strcat(str, "  INVALID HEADER RECORD\n");
	}else if( err.type == INV_RECORD ){		strcat(str, "  INVALID RECORD\n");
	}else if( err.type == OTHER_ERROR ){				strcat(str, "  ASCII\n");
	}
	
	if(err.line != -1){
		
		strcat(str, "  At line: ");
		snprintf(tmp, 50, "%d", err.line);
		strcat(str, tmp);
		strcat(str, "\n");
		
	}
	
    free(tmp);
	return str;
	
}

void deleteGEDCOM(GEDCOMobject* obj){
	
	clearList( &obj->individuals );
	clearList( &obj->families );
	deleteSubmitter( &obj->submitter );
	deleteHeader( &obj->header );
	free( obj );
	obj = NULL;
	
}



List getDescendants(const GEDCOMobject* familyRecord, const Individual* person){

	List descendants = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );

	if( familyRecord == NULL || person == NULL ){

		return descendants;

	}
	recurDescendants( familyRecord, &descendants, person );
	
	return descendants;

}

GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj){
	printf("\tC Write\t Entering\n");
	char* toWrite = malloc( sizeof(char) * 1024 );
	strcpy( toWrite, "" );
	char* tmp = malloc( sizeof(char) * 1024 );
	
	writeGEDCOMLine( "0", "HEAD", NULL, &toWrite );
	writeGEDCOMLine( "1", "SOUR", obj->header->source, &toWrite );
	writeGEDCOMLine( "1", "GEDC", NULL, &toWrite);
	
	sprintf( tmp , "%.1f", obj->header->gedcVersion );
	
	writeGEDCOMLine( "2", "VERS", tmp, &toWrite);
	writeGEDCOMLine( "2", "FORM", "LINEAGE-LINKED", &toWrite);
	
	if( obj->header->encoding == ANSEL ){ strcpy( tmp, "ANSEL" );}
	else if( obj->header->encoding == ASCII ){ strcpy( tmp, "ASCII" );
	}else if( obj->header->encoding == UNICODE ){ strcpy( tmp, "UNICODE" );
	}else if( obj->header->encoding == UTF8 ){ strcpy( tmp, "UTF-8" );
	}else{
		printf("\tC Write\t Bad Encoding\n");
		return createERROR( WRITE_ERROR, -1 );
	}
	
	writeGEDCOMLine( "1", "CHAR", tmp, &toWrite);
	writeGEDCOMLine( "1", "SUBM", generateRef("SUB", 1) , &toWrite);

	
	writeGEDCOMLine( "0", generateRef( "SUB", 1 ), "SUBM", &toWrite );
	writeGEDCOMLine( "1", "NAME", obj->submitter->submitterName, &toWrite );
	

	char* GEDCOMrep;
	
	ListIterator iter = createIterator( obj->individuals );
	void* elem;
	Individual* indi;
	
	while( (elem = nextElement(&iter)) != NULL ){
		
		indi = (Individual*)elem;
		GEDCOMrep = indiToGEDCOM( indi );
		safeStrCat( GEDCOMrep, &toWrite );
		
	}
	
	iter = createIterator( obj->families );
	Family* fam;
	
	while( (elem = nextElement(&iter)) != NULL ){
		
		fam = (Family*)elem;
		GEDCOMrep = famToGEDCOM( fam );
		safeStrCat( GEDCOMrep, &toWrite );
		
	}
	
	writeGEDCOMLine( "0", "TRLR", NULL, &toWrite );
	
	printf("\tC Write\t Opening File\n");
	FILE* fp = fopen (fileName,"w");
	fputs (toWrite, fp);
	fclose( fp );
	printf("\tC Write\t File Wrote and Closed\n");
	return createERROR( OK, -1 );
	
}

List getDescendantListN( const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen){
	
	List descendantsByGen = initializeList( &printList, &noFree, &compareList );
	
	if( familyRecord == NULL || person == NULL ){ return descendantsByGen; }
	
	if( maxGen == 0 ){ maxGen = 1024; }
	
	int curGen = 1;
	
	recurDescendantsN( familyRecord, &descendantsByGen, person, maxGen, curGen );
	
	return descendantsByGen;

}

List getAncestorListN( const GEDCOMobject* familyRecord, const Individual* person, int maxGen){
	
	List ancestorsByGen = initializeList( &printList, &noFree, &compareList );
	
	if( familyRecord == NULL || person == NULL ){ return ancestorsByGen; }
	
	if( maxGen == 0 ){ maxGen = 1024; }
	
	int curGen = 1;
	
	recurAncestorsN( familyRecord, &ancestorsByGen, person, maxGen, curGen );
	
	return ancestorsByGen;
	
}

char* indToJSON(const Individual* ind){
	
	/*"{"givenName":"givenName value","surname":"surname value"}"*/
	
	char* JSONstr = malloc( sizeof( char ) * 1024 );
	
	strcpy( JSONstr, "" );
	
	if( ind == NULL ){
		return JSONstr;
	}
	
	safeStrCat( "{\"givenName\":\"", &JSONstr ); // "{"givenName":"
	
	if( ind->givenName != NULL ){
		safeStrCat( ind->givenName, &JSONstr ); // "{"givenName":"GIVN
	}
	
	safeStrCat( "\",\"surname\":\"", &JSONstr ); // "{"givenName":"GIVN","surname":"
	
	if( ind->surname != NULL ){
		safeStrCat( ind->surname, &JSONstr ); // "{"givenName":"GIVN","surname":"SURN
	}
	
	safeStrCat( "\"}", &JSONstr ); // "{"givenName":"GIVN","surname":"SURN"}"
	
	return JSONstr;
}

Individual* JSONtoInd(const char* str){
	
	if( str == NULL ){
		return NULL;
	}
	
	Individual* indi = createIndividual();
	char* givn = malloc( sizeof( char ) * 256 );
	char* surn = malloc( sizeof( char ) * 256 );
	strcpy( givn, "" );
	strcpy( surn, "" );
	
	int givn_i = 0;
	int surn_i = 0;
	int i = 14;
	
	while( str[ i ] != '\"' ){
		givn[ givn_i ] = str[ i ];
		i ++;
		givn_i ++;
	}
	
	givn[ givn_i ] = '\0';
	i += 13;
	
	while( str[ i ] != '\"' ){
		surn[ surn_i ] = str[ i ];
		i ++;
		surn_i ++;
	}
	
	surn[ surn_i ] = '\0';
	
	indi->givenName = givn;
	indi->surname = surn;
	
	return indi;
}

GEDCOMobject* JSONtoGEDCOM(const char* str){
	
	if( str == NULL ){
		return NULL;
	}
	
	
	GEDCOMobject* GEDobj = malloc( sizeof( GEDCOMobject ) );
	
	GEDobj->families = initializeList( &printFamily, &FreeFamily, &compareFamilies );
	GEDobj->individuals = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
	GEDobj->header = createHeader();
	
	char* source = malloc( sizeof( char ) * 256 );
	char* gedc_str = malloc( sizeof( char ) * 256 );
	char* encoding_str = malloc( sizeof( char ) * 256 );
	char* subName = malloc( sizeof( char ) * 256 );
	char* subAddr = malloc( sizeof( char ) * 256 );
	
	strcpy( source, "" );
	strcpy( gedc_str, "" );
	strcpy( encoding_str, "" );
	strcpy( subName, "" );
	strcpy( subAddr, "" );
	
	int ctr = 0;
	int i = 11;
	
	while( str[ i ] != '\"' ){
		source[ ctr ] = str[ i ];
		i ++;
		ctr ++;
	}
	source[ ctr ] = '\0';
	ctr = 0;
	i += 17;
	
	while( str[ i ] != '\"' ){
		gedc_str[ ctr ] = str[ i ];
		i ++;
		ctr ++;
	}
	gedc_str[ ctr ] = '\0';
	ctr = 0;
	i += 14;
	
	while( str[ i ] != '\"' ){
		encoding_str[ ctr ] = str[ i ];
		i ++;
		ctr ++;
	}
	encoding_str[ ctr ] = '\0';
	ctr = 0;
	i += 13;
	
	while( str[ i ] != '\"' ){
		subName[ ctr ] = str[ i ];
		i ++;
		ctr ++;
	}
	subName[ ctr ] = '\0';
	ctr = 0;
	i += 16;
	
	while( str[ i ] != '\"' ){
		subAddr[ ctr ] = str[ i ];
		i ++;
		ctr ++;
	}
	subAddr[ ctr ] = '\0';
	ctr = 0;
	
	CharSet encoding;
	
	if ( strEqu( encoding_str, "ANSEL" ) == true ){ 			encoding = ANSEL;
	}else if( strEqu( encoding_str, "UTF-8" ) == true ){ 	encoding = UTF8;
	}else if( strEqu( encoding_str, "UNICODE" ) == true ){ 	encoding = UNICODE;
	}else if( strEqu( encoding_str, "ASCII" ) == true ){		encoding = ASCII;
	}
	
	GEDobj->submitter = createSubmitter( strlen( subAddr ) );
	double vers = atof( gedc_str );
	
	GEDobj->header->encoding = encoding;
	GEDobj->header->gedcVersion = vers;
	strcpy(GEDobj->header->source, source);
	
	strcpy(GEDobj->submitter->address, subAddr);
	strcpy(GEDobj->submitter->submitterName, subName);
	
	GEDobj->header->submitter = GEDobj->submitter;
	
	return GEDobj;
	
}

char* GEDCOMtoJSON( GEDCOMobject* obj ){
	
	/*{"source":"val","GEDCversion":"val","encoding":"val","subname":"val","subaddr":"val","numind":"val","numfam":"val"}*/
	
	char* JSONstr = malloc( sizeof( char ) * 1024 );
	char* tmp = malloc( sizeof( char ) * 1024 );
	
	strcpy( JSONstr, "" );
	
	if( obj == NULL ){
		return JSONstr;
	}
	
	safeStrCat( "{\"source\":\"", &JSONstr ); // {"source":"
	
	safeStrCat( obj->header->source, &JSONstr ); // "{"source":"val
	
	safeStrCat( "\",\"GEDCOMversion\":\"", &JSONstr ); // "{"source":"val","GEDCOMversion":"
	
	sprintf( tmp , "%.1f", obj->header->gedcVersion ); 
	
	safeStrCat( tmp, &JSONstr ); // "{"source":"val","GEDCOMversion":val
	
	safeStrCat( "\",\"encoding\":\"", &JSONstr ); // "{"source":"val","GEDCOMversion":val","encoding":"
	
	if( obj->header->encoding == ANSEL ){ strcpy( tmp, "ANSEL" );}
	else if( obj->header->encoding == ASCII ){ strcpy( tmp, "ASCII" );
	}else if( obj->header->encoding == UNICODE ){ strcpy( tmp, "UNICODE" );
	}else if( obj->header->encoding == UTF8 ){ strcpy( tmp, "UTF-8" );
	}
	
	safeStrCat( tmp, &JSONstr ); // "{"source":"val","GEDCOMversion":val","encoding":"val
	
	safeStrCat( "\",\"subname\":\"", &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"
	
	if( obj->submitter->submitterName != NULL ){
		safeStrCat( obj->header->submitter->submitterName, &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val
	}
	
	int numFam = obj->families.length;
	int numInd = obj->individuals.length;
	
	safeStrCat( "\",\"numFam\":\"", &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"numFam":"
	
	sprintf( tmp , "%d", numFam );
	
	safeStrCat( tmp, &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"numFam":"val
	
	safeStrCat( "\",\"numInd\":\"", &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"numFam":"val,"numInd":"
	
	sprintf( tmp , "%d", numInd );
	
	safeStrCat( tmp, &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"numFam":"val,"numInd":"val
	
	
	
	/*
	safeStrCat( "\",\"subaddr\":\"", &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"subaddr":"
	
	if( obj->submitter->address != NULL ){
		safeStrCat( obj->header->submitter->address, &JSONstr ); //"{"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"subaddr":"val
	}
	* */
	
	safeStrCat( "\"}", &JSONstr ); // {"source":"val","GEDCOMversion":val","encoding":"val","subname":"val,"subaddr":"val"}
	
	return JSONstr;
	
}

char* iListToJSON(List iList){
	
	char* JSONlist = malloc( sizeof(char) * 1024);
	
	if( iList.head == NULL ){
		strcpy(JSONlist, "[]");
		return JSONlist;
	}else{
		strcpy(JSONlist, "[");
	}
	
	ListIterator iter = createIterator( iList );
	Individual* indi;
	void* elem;
	char* toAppend;
	
	bool first = true;
	
	while( (elem = nextElement(&iter) ) != NULL){
		
		if( first == true ){
			first = false;
		}else{
			safeStrCat( ",", &JSONlist  );
		}
		
		indi = (Individual*)elem;
		toAppend = indToJSON( indi );
		safeStrCat( toAppend, &JSONlist );
		
	}
	safeStrCat( "]", &JSONlist );
	return JSONlist;
	
}

void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded){
	
	if( obj == NULL || toBeAdded == NULL ){
		return;
	}
	
	insertBack( &obj->individuals, (void*)toBeAdded );
	return;
	
}
	
char* gListToJSON(List gList){
	
	char* JSONlist = malloc( sizeof(char) * 1024);
	
	if( gList.head == NULL ){
		strcpy(JSONlist, "[]");
		return JSONlist;
	}else{
		strcpy(JSONlist, "[");
	}
	
	ListIterator iter = createIterator( gList );
	List* gen;
	void* elem;
	char* toAppend;
	
	bool first = true;
	
	while( (elem = nextElement(&iter) ) != NULL){
		
		if( first == true ){
			first = false;
		}else{
			safeStrCat( ",", &JSONlist  );
		}
		
		gen = (List *)elem;
		toAppend = iListToJSON( *gen );
		safeStrCat( toAppend, &JSONlist );
		
	}
	safeStrCat( "]", &JSONlist );
	return JSONlist;
	
}

ErrorCode validateGEDCOM(const GEDCOMobject* obj){
	
	if( obj->header == NULL || obj->submitter == NULL ){ return INV_GEDCOM; }
	
	if( obj->header->source == NULL || obj->header->submitter == NULL ){ return INV_GEDCOM; }
	
	if( strEqu(obj->submitter->submitterName, "") == true ){ return INV_RECORD; }
	
	ListIterator iter = createIterator( obj->families );
	Family* fam;
	void* elem;
	
	while( (elem = nextElement(&iter) ) != NULL){
		fam = (Family*)elem;
		if( fam == NULL ){
			return INV_RECORD;
		}
	}
	
	iter = createIterator( obj->individuals );
	Individual* ind;
	
	while( (elem = nextElement(&iter) ) != NULL){
		ind = (Individual*)elem;
		if( ind == NULL ){
			return INV_RECORD;
		}
	}
	
	return OK;
}

/* - - - - - - - - - - - - - -*/
/* - - - LIST FUNCTIONS - - - */
/* - - - - - - - - - - - - - -*/


void deleteEvent(void* toBeDeleted){}
int compareEvents(const void* first,const void* second){ return 0; }
char* printEvent(void* toBePrinted){ 

	char* tmp = malloc( sizeof(char) * 256 );
	strcpy(tmp, "");
	return tmp;

}





int compareIndividuals(const void* first,const void* second){
	
	/* Alphabetical order by surname, if equal, resort to given name */
	
	Individual* firstIndi = (Individual*)first;
	Individual* secondIndi = (Individual*)second;
	
	int cmp = strcmp( firstIndi->surname, secondIndi->surname );
	
	if( cmp < 0 ){
		return -1;
	}else if( cmp > 0 ){
		return 1;
	}else{
		
		cmp = strcmp( firstIndi->givenName, secondIndi->givenName );
		
		if( cmp < 0 ){
			return -1;
		}else if( cmp > 0 ){
			return 1;
		}else{
			return 0;
		}
		
	}
	
	return 0;

}

Individual* getIndFromGed(char* firstName, char* lastName, GEDCOMobject* obj){
	
	printf("\tC SEARCH:\t Entered Search\n");
	
	/* Return the pointer to a Individual in a GEDCOM object */
	
	ListIterator iter = createIterator( obj->individuals );
	
	printf("\tC SEARCH:\t Made Iterator\n");
	
	Individual* ind = NULL;
	void* elem;
	
	printf("\tC SEARCH:\t INIT Variables Complete\n");
	
	printf("\tC SEARCH:\t First Name: %s\n", firstName);
	printf("\tC SEARCH:\t Last Name: %s\n", lastName);
	
	while( (elem = nextElement(&iter) ) != NULL){
		ind = (Individual*)elem;
		
		if( firstName != NULL && lastName != NULL && ind->givenName != NULL && ind->surname != NULL){
			
			
			printf("\tC SEARCH:\t Comparing GIVN %s and %s\n", firstName, ind->givenName);
			printf("\tC SEARCH:\t Comparing SURN %s and %s\n", lastName, ind->surname);
			
			
			if( strEqu( firstName, ind->givenName ) && strEqu( lastName, ind->surname ) ){
				
				printf("\tC SEARCH:\t Match Found\n");
				return ind;
			}
			
		}else if( firstName != NULL && ind->givenName != NULL ){
			
			if( strEqu( firstName, ind->givenName ) ){
				printf("\tC SEARCH:\t Match Found\n");
				return ind;
			}
			
		}else if( lastName != NULL && ind->surname != NULL ){
			
			if( strEqu( lastName, ind->surname ) ){
				printf("\tC SEARCH:\t Match Found\n");
				return ind;
			}
			
		}
		
	}
	
	printf("\tC SEARCH:\t No Match Found. Fell out Bottom.\n");
	return ind;

}

char* printIndividual(void* toBePrinted){
	
    char* TempString;
	char* str = malloc( sizeof(char) * 2048 );
	strcpy( str, "" );

	Individual* i = (Individual*)toBePrinted;

	strcat( str, "\n[ INDIVIDUAL ]\n" );
	strcat( str, "  NAME: " );
    
    if (i -> givenName != NULL) {
        
        strcat( str, i -> givenName );
    }
    
    if (i -> surname != NULL) {
        strcat( str, " ");
        strcat( str, i -> surname );
    }
    
	strcat( str, "\n  Events:" );
	/* TODO: Get Events */
	
	strcat( str, "\n  Other Fields:" );
    
    TempString = toString( i -> otherFields );
    str = realloc(str, strlen(str) + strlen(TempString) + 1);
	strcat( str, TempString);
    free(TempString);
	
	return str;

}

void deleteIndividual(void* toBeDeleted){

	Individual* i = (Individual*)toBeDeleted;
	
	if( i->surname != NULL ){
		free( i->surname );
	}
	
	if( i->givenName != NULL ){
		free( i->givenName );
	}

	clearList( &i->otherFields );
	clearList( &i->families );
	free( i );
	i = NULL;

}

void deleteField(void* toBeDeleted){

	Field* f = (Field*)toBeDeleted;
	free( f );
	
}

int compareFields(const void* first,const void* second){

	return 0;

}

char* printField(void* toBePrinted){
	
	char* str = malloc( sizeof(char) * 1024 );
	strcpy( str, "" );
	Field* f = (Field*)toBePrinted;

	strcat( str, "  [ FIELD ]\n" );
	strcat( str, "    TAG: " );
    
    if (f -> tag != NULL){

        strcat( str, f -> tag); 

    } else {

//        strcat( str, "Missing");
    }
    
	strcat( str, "\n    VALUE: ");
	
	if( f -> value != NULL ){

		strcat( str, f -> value);	

    } else {

//        strcat( str, "Missing");

    }
	
	return str;

}

void deleteFamily(void* toBeDeleted){

	Family* f = (Family*)toBeDeleted;
	clearList( &f->children);
	clearList( &f->otherFields );
	free( f );
	f = NULL;
	
}

int compareFamilies(const void* first,const void* second){
	
	Family* famA = (Family*)first;
	Family* famB = (Family*)second;
	
	if( famA->husband->surname == NULL && famB->husband->surname == NULL ){
		return 0;
	}else if( famA->husband->surname == NULL){
		return -1;
	}else if( famB->husband->surname == NULL){
		return 1;
	}
	
	int cmp = strcmp( famA->husband->surname, famB->husband->surname );
	
	if( cmp < 0 ){
		return -1;
	}else if( cmp > 0 ){
		return 1;
	}else{
		return 0;
	}
}

char* printFamily(void* toBePrinted){
	
    char* TempString;
	char* str = malloc( sizeof(char) * 2048);
	strcpy( str, "" );
	
	Family* f = (Family*)toBePrinted;
	
	strcat( str, "\n[ FAMILY ]" );
	
	
	if( f -> wife != NULL ){
		
		strcat( str, "\n  Wife: " );
        
        if (f -> wife -> givenName != NULL) {

            strcat( str, f -> wife -> givenName ); 
        
        }
        if (f -> wife -> surname != NULL) {

            strcat( str, " " ); 
            strcat( str, f -> wife -> surname ); 

        }
	}
	

	if( f -> husband != NULL ){
		
        strcat( str, "\n  Husband: " );
        
        if (f -> husband -> givenName != NULL) {

            strcat( str, f -> husband -> givenName);

        }
        if (f -> husband -> surname != NULL) {

            strcat( str, " ");
            strcat( str, f -> husband -> surname);
        }
	}
	

	strcat( str, "\n  Family's Childern: " );
  
    TempString = toString( f->children ) ;
    str = realloc(str, strlen(str) + strlen(TempString) + 100);
	strcat( str, TempString);
    free(TempString);

    
	strcat( str, "\n  Family's Other Fields: " );
    TempString = toString(f -> otherFields ) ;
    int NewLen = strlen(str) + strlen(TempString) + 1;
    str = realloc(str, NewLen);
	strcat( str, TempString);
    free(TempString);

	return str;

}

void deleteGeneration(void* toBeDeleted){

	List* l = (List*)toBeDeleted;
	clearList( l );

}
int compareGenerations(const void* first,const void* second){

	return 0;

}
char* printGeneration(void* toBePrinted){

	char* tmp = malloc( sizeof(char) * 256 );
	strcpy(tmp, "");
	return tmp;

}

char* JSONgetIndList( char* fileName ){
	
	GEDCOMobject* obj;
	
	createGEDCOM(fileName, &obj);
	
	char* toReturn = iListToJSON( obj->individuals );

	return toReturn;
	
}
char* JSONgetGEDCOM( char* filename ){
	GEDCOMobject* GEDCOMobj;
	createGEDCOM( filename, &GEDCOMobj );
	char* JSONstr = GEDCOMtoJSON( GEDCOMobj );
	return JSONstr;
}

char* JSONvalidate( char* filename ){
	
	
	printf("C Validate:\t%s\n", filename);
	GEDCOMobject* obj;
	GEDCOMerror err = createGEDCOM( filename, &obj );
	
	char* toReturn = malloc(sizeof(char) * 256);
	
	if( err.type != 0 ){
		
		strcpy(toReturn, printError(err));
		return toReturn;
	}else{
		strcpy(toReturn, "OK");
		return toReturn;
	}
	
}

char* JSONgetDesList( char* filename, char* name, int num ){
	
	GEDCOMobject* obj;
	printf( "%s\n", printError( createGEDCOM(filename, &obj) ));
	
	char* first = malloc(sizeof(char) * 128);
	char* last = malloc(sizeof(char) * 128);
	
	first = strtok( name, " " );
	last = strtok( NULL, " " );
	
	printf("C:\t Full: %s\n", name);
	printf("C:\t First: %s\n", first);
	printf("C:\t Last: %s\n", last);
	
	/* My getDescendantsListN function uses pointer logic to determine matches, therefore we must send in a pointer to obj.*/
	
	printf("C:\t Executing IND search\n");
	Individual* ind = getIndFromGed(first, last, obj);
	
	printf("C:\t Generating JSON GEN list\n");
	List genList = getDescendantListN( obj, ind, num);
	
	printf("C:\t Returning:\n\n%s", gListToJSON(genList));
	return gListToJSON(genList);
	
}

char* JSONgetAnList( char* filename, char* name, int num ){
	
	GEDCOMobject* obj;
	printf( "%s\n", printError( createGEDCOM(filename, &obj) ));
	
	char* first = malloc(sizeof(char) * 128);
	char* last = malloc(sizeof(char) * 128);
	
	first = strtok( name, " " );
	last = strtok( NULL, "" );
	
	printf("C:\t Full: %s\n", name);
	printf("C:\t First: %s\n", first);
	printf("C:\t Last: %s\n", last);
	
	/* My getDescendantsListN function uses pointer logic to determine matches, therefore we must send in a pointer to obj.*/
	
	printf("C:\t Executing IND search\n");
	Individual* ind = getIndFromGed(first, last, obj);
	
	printf("C:\t Generating JSON GEN list\n");
	List genList = getAncestorListN( obj, ind, num);
	
	printf("C:\t Returning:\n\n%s", gListToJSON(genList));
	return gListToJSON(genList);
	
}

void createNewFile( char* fileName, char* subName, char* subAddr, char* charSet){
	
	/* Malloc Object */
	GEDCOMobject* obj = malloc( sizeof(GEDCOMobject) );
	
	/* First, create Header */
	Header* head = createHeader();
	
	/* Set encoding */
	CharSet encoding;
	if ( strEqu( charSet, "ANSEL" ) == true ){ 			encoding = ANSEL;
	}else if( strEqu( charSet, "UTF-8" ) == true ){ 	encoding = UTF8;
	}else if( strEqu( charSet, "UNICODE" ) == true ){ 	encoding = UNICODE;
	}else if( strEqu( charSet, "ASCII" ) == true ){		encoding = ASCII;
	}
	head->encoding = encoding;
	
	/* Set Version */
	head->gedcVersion = 5.5;
	
	/* Set Source */
	strcpy(head->source, "Genealogy App");
	
	/* Next, create the submitter */
	Submitter* sub = createSubmitter( sizeof(char) * strlen( subAddr ) );
	
	/* Set submitter name */
	strcpy( sub->submitterName, subName );
	
	/* Create empty lists for GEDCOMobj */
	List indiList = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
	List famList = initializeList( &printFamily, &FreeFamily, &compareFamilies );
	
	/* Links things up to main GEDCOMobj */
	head->submitter = sub;
	obj->submitter = sub;
	obj->header = head;
	
	obj->families = famList;
	obj->individuals = indiList;
	
	printf( "%s\n",printError(writeGEDCOM(fileName, obj)));
	printf("C\t: Wrote File to %s\n", fileName);
	return;
	
}

void addIndFromName(char* filename, char* first, char* last){
	
	Individual* ind = createIndividual();
	ind->surname = last;
	ind->givenName = first;
	
	GEDCOMobject* obj;
	createGEDCOM(filename, &obj);
	
	addIndividual(obj, ind);
	
	writeGEDCOM( filename, obj );
	
}

char* getString(){
	
	char* str = malloc( sizeof(char) * 32 );
	strcpy( str, "Test C String\n" );
	return( str );
	
}

char* returnString( char* str ){
	
	return( str );
	
}

bool file_exists(const char * filename)
{
	FILE * file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}
