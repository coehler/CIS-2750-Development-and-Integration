#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "GEDCOMparser.h"
#include "LinkedListAPI.h"
#include "GEDCOMutilities.h"

extern GEDCOMerror globalGEDCOMerror;

/* - - - - - - - - - - - - - */
/* - - - MISC FUNCIONS - - - */
/* - - - - - - - - - - - - - */

GEDCOMerror createERROR(ErrorCode type, int line){

//	GEDCOMerror GEDerr;
	globalGEDCOMerror.type = type;
	globalGEDCOMerror.line = line;
	return globalGEDCOMerror;
}

Header* createHeader( ){

	Header* GEDhead = malloc( sizeof(Header) );
    strcpy(GEDhead -> source,"");
    GEDhead -> gedcVersion = -1;
    GEDhead -> submitter = NULL;
	GEDhead -> otherFields = initializeList( &printField, &deleteField, &compareFields );
    
	return GEDhead;

}

Submitter* createSubmitter( size_t len ){

	Submitter* GEDsub = malloc( sizeof( Submitter ) +  len * sizeof( GEDsub->address[0] ) );
	strcpy(GEDsub->submitterName, "");
	GEDsub -> otherFields = initializeList( &printField, &deleteField, &compareFields );
	return GEDsub;
}

Family* createFamily( ){

	Family* GEDfam = malloc( sizeof(Family) );
	GEDfam -> wife = NULL;
	GEDfam -> husband = NULL;
	GEDfam -> children = initializeList( &printIndividualNested, &noFree, &compareIndividuals );
	GEDfam -> otherFields = initializeList( &printFieldNested, &deleteField, &compareFields );
	return GEDfam;
}

Individual* createIndividual( ){

	Individual* GEDindi = malloc( sizeof(Individual) );
	GEDindi -> surname = NULL;
	GEDindi -> givenName = NULL;
    GEDindi -> events =  initializeList( &printFamily, &noFree, &compareFamilies );
	GEDindi -> families = initializeList( &printFamily, &noFree, &compareFamilies );
	GEDindi -> otherFields = initializeList( &printField, &deleteField, &compareFields );
	return GEDindi;
}

char * strdup(const char * s){

  if (s == NULL){
        return NULL;
  } else {
      size_t len = 1 + strlen( s );
      char* p = malloc( len );
      return p ? memcpy(p, s, len) : NULL;
  }
}

char * strdup2(const char * s){

  if (s == NULL){
        return NULL;
  } else {
      size_t len = 1 + strlen( s );
      char* p = malloc( len );
      return p ? memcpy(p, s, len) : NULL;
  }
}

bool strEqu(char * a, char * b){
	
	if( strcmp(a, b) == 0){

		return true;

	}else{

		return false;

	}

}
/* - - - - - - - - - - - - - */
/* - - - LIST FUNCIONS - - - */
/* - - - - - - - - - - - - - */



char* printFieldNested(void* toBePrinted){
	
	char* str = malloc( sizeof(char) * 1024 );
	strcpy( str, "" );
	Field* f = (Field*)toBePrinted;

	strcat( str, "    [ FIELD ]\n" );
	strcat( str, "      TAG: " );
    
    
	if( f -> tag != NULL ){

        strcat( str, f -> tag);

    } else {

//        strcat( str, "Missing");

    }
    
	strcat( str, "\n      VALUE: ");
	
	if( f -> value != NULL ){

		strcat( str, f -> value);
        
    } else {

//        strcat( str, "Missing");
    }
    
	return str;

}




bool individualEqu( const Individual* a, const Individual* b ){

	if( strEqu( a->givenName, b->givenName ) && strEqu( a->surname, a->surname ) ){

		return true;

	}else{

		return false;

	}

}

char* printIndividualNested(void* toBePrinted){
	
	char* str = malloc( sizeof(char) * 2048);
	strcpy( str, "" );

	Individual* i = (Individual*)toBePrinted;

	strcat( str, "    [ INDIVIDUAL ]\n" );
	strcat( str, "      NAME: " );
    
    if (i -> givenName != NULL) {

        strcat( str, i -> givenName );
    
    }
    if ( i -> surname != NULL) {
	
        strcat( str, " ");
        strcat( str, i -> surname );
    }

	return str;

}



void deleteLink(void* toBeDeleted){

	Link* l = (Link*)toBeDeleted;
	free(l->linkStr);
	free(l->data);
	free( l );
	l = NULL;

}

void deleteHeader(void* toBeDeleted){
	
//	Header* h = (Header*)toBeDeleted;
//	clearList( &h->otherFields );
//	free( h );
//    h = NULL;
	return;
	
}

void deleteSubmitter(void* toBeDeleted){
	
//	Submitter* s = (Submitter*)toBeDeleted;
//	clearList( &s->otherFields );
//	free( s->submitterName); 
//	free( s );
	return;
	
}

void noFree(void* toBeDeleted){

}

void FreeIndividual(void* toBeDeleted){
    
Individual * s = (Individual*)toBeDeleted;

if (s->givenName != NULL) {
    free(s->givenName);
    s->givenName = NULL;
}
if (s->surname != NULL) {
    free(s->surname);
    s->surname = NULL;
}

clearList(&s->events);
clearList(&s->families);
clearList(&s->otherFields);
    
}

void FreeFamily(void* toBeDeleted){
    
Family * s = (Family*)toBeDeleted;

/*
 * Individuals have already been freed in FreeIndividual
 * Just NULL wife and husband
 */

s->wife = NULL;
s->husband = NULL;

clearList(&s->children);
clearList(&s->otherFields);
    
}

void FreeLinkStr(void* toBeDeleted){
/*
 * Only free linkStr.  Data points to actual Family and Individual records used by GEDCOMObj
 */

Link * s = (Link*)toBeDeleted;
free(s->linkStr);

}

int compareLink(const void* first,const void* second){

	return 0;

}

int compareList(const void* first,const void* second){

	return 0;

}

char* printLink(void* toBePrinted){
	

	Link l = *(Link*)toBePrinted;

	printf("[LINK]: %s\n", l.linkStr);

	return("LINK");

}

char* printSubmitter(void* toBePrinted){
	
	char* str = malloc( sizeof(char) * 2048 );
	strcpy( str, "" );
	Submitter* s = (Submitter*)toBePrinted;

	strcat( str, "    [ SUBMITTER ]\n" );
	strcat( str, "      NAME: " );
    
    if (s->submitterName != NULL) {

        strcat( str, (char*)s->submitterName);

    }
	
	if( s->address != NULL ){

		strcat( str, "\n    ADDRESS: ");
		strcat( str, s->address);	

	}
	
	return str;

}
/* - - - - - - - - - - - - - - - - - - - - */
/* - - - FIRST PASS PARSING FUNCIONS - - - */
/* - - - - - - - - - - - - - - - - - - - - */

char* parseAddr( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int* index ){
	
	char* addr = malloc( sizeof(char) * 512 );
	int curLvl = -1;
	int u;
	int i = *index;
	
	while( i < size ){

		if ( curLvl == 0 ){
			
			*index = i;
			return addr;

		}else{
			
			u = 2;
			while( ( fieldElmntArr[ i ][ u ]) != NULL ){
				
				strcat(addr, fieldElmntArr[ i][ u ] );
				strcat(addr, "\n");
				u++;
				
			}
			
		}
		
		i ++;
		curLvl = atoi( fieldElmntArr[ *index ][0]);

	}
	
	*index = i;
	return addr;
}

Header* parseHeader( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index ){

	Field* GEDfield;
	Header* GEDhead = createHeader();
	int curLvl = -1;
	int i = index;
	CharSet encoding;
	char* curPath;
    bool FlagEncoding = false;
    bool FlagSource = false;
    bool FlagGEDCVersion = false;
    bool FlagSubmitter = false;

	while ( i < size ){

		curPath = pathArr[i];

		if( strEqu( curPath, "HEADSOUR") ){

			/**
			 * { get SOURce information }
			 */
			
            FlagSource = true;
			strcpy( GEDhead -> source, fieldElmntArr[ i ][ 2 ] ); 

		}else if ( strEqu( curPath, "HEADGEDCVERS") ){

			/**
			 * { get VERSion number }
			 */

            FlagGEDCVersion = true;
            float vers = atof(fieldElmntArr[ i ][ 2 ]);
			
			GEDhead->gedcVersion = vers;

		}else if ( strEqu( curPath, "HEADCHAR") ) {

			/**
			 * { get CHARacter set -- from enum }
			 */
            FlagEncoding = true;
            
			if ( strEqu( fieldElmntArr[ i ][ 2 ], "ANSEL" ) == true ){ 		encoding = ANSEL;
			}else if( strEqu( fieldElmntArr[ i ][ 2 ], "UTF-8" ) == true ){ 	encoding = UTF8;
			}else if( strEqu( fieldElmntArr[ i ][ 2 ], "UNICODE" ) == true ){ encoding = UNICODE;
			}else if( strEqu( fieldElmntArr[ i ][ 2 ], "ASCII" ) == true ){	encoding = ASCII;
			}else{
                createERROR(INV_RECORD, i);
                return NULL;
            }

			GEDhead -> encoding = encoding;
			
		}else if ( strEqu( curPath, "HEAD") ) {

		}else if ( strEqu( curPath, "HEADSUBM") ) {

			FlagSubmitter = true;

		}else if ( curLvl == 0 ){

            /*
             * Test for specific missing data errors
             */
             if(FlagSubmitter == false){
             	createERROR(INV_HEADER,i);
             	return NULL;
             }
             if (FlagEncoding == false){
                 createERROR(INV_RECORD, i);
                 return NULL;
             }        
             if (FlagSource == false || FlagGEDCVersion == false){
                 createERROR(INV_HEADER, i);
                 return NULL;
             }  

			return GEDhead;

		}else if ( strEqu( curPath, "HEADSOURNAME") ||
                   strEqu( curPath, "HEADSOURVERS") ||
                   strEqu( curPath, "HEADGEDC") ||
                   strEqu( curPath, "HEADDATEFORM") ||
                   strEqu( curPath, "HEADGEDCFORM") ||
                   strEqu( curPath, "HEADDATE") ||
                   strEqu( curPath, "HEADFILE"))
        {
            GEDfield = malloc( sizeof(Field) );
            GEDfield -> tag = fieldElmntArr[ i ][ 1 ];
        
            if( fieldElmntArr[ i ][ 2 ] != NULL ){
                GEDfield -> value = fieldElmntArr[ i ][ 2 ];
            }else{
                GEDfield -> value = NULL;
            }
            insertBack( &GEDhead->otherFields, GEDfield );
            
		}  else {
            
            /*
             * Unknown header tag
             */
                createERROR(INV_RECORD, i);
                return NULL;
        }
		
		i ++;
		curLvl = atoi( fieldElmntArr[i][0]);
	}
   
    if (FlagSource == false){
        createERROR(INV_HEADER, i);
        return NULL;
    }
    

	return GEDhead;
}

Submitter* parseSubmitter( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index ){

	Field* GEDfield;
	Submitter* GEDsub = createSubmitter(512);
	int curLvl = -1;
	int i = index;
    bool FlagSubmitter = false;

	char* curPath;

	while( i < size ){

		curPath = pathArr[i];
        
        if( strEqu( curPath, "SUBMNAME") ){

			/**
			 * { get NAME of submitter }
			 */
			
			strcpy( GEDsub->submitterName, strdup2(fieldElmntArr[i][2]));

		}else if ( strEqu( curPath, "SUBMADDR") ){

			/**
			 * { start parsing the address }
			 */
			
			//strcpy( GEDsub -> address, parseAddr( pathArr, fieldElmntArr, size, &i ) );
            
		}else if ( curLvl == 0 ){
           
            if ( FlagSubmitter != true) {
                createERROR(INV_HEADER, i);
                return NULL;
            }

			return GEDsub;

		}else if ( strEqu( curPath, "SUBM"))
        {
            FlagSubmitter = true;
			GEDfield = malloc( sizeof(Field) );
			GEDfield -> tag = fieldElmntArr[ i ][ 1 ];
			GEDfield -> value = fieldElmntArr[ i ][ 2 ];

			insertBack( &GEDsub->otherFields, GEDfield );
			
		}  else {
            
            /*
             * Unknown tag
             */
            createERROR(INV_RECORD, i);
            return NULL;
        }
		
		i ++;
		curLvl = atoi( fieldElmntArr[i][0]);
	}
	
	return GEDsub;
}

Link* parseFamilyLink( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index ){

	Field* GEDfield;
	int curLvl = -1;
	int i = index;
	
	Link* GEDlink = malloc( sizeof(Link) );
	char* linkStr = malloc( sizeof(char) * 32 );
	Family* GEDfam = createFamily();

	while( i < size ){

		if( curLvl == 0 ){

			break;
	
		}else if ( fieldElmntArr[ i ][ 1 ][ 0 ] == '@' ){

			strcpy( linkStr, fieldElmntArr[ i ][ 1 ] );

		}else{
			
			if( (strEqu(fieldElmntArr[ i ][ 1 ], "HUSB") != true) && (strEqu(fieldElmntArr[ i ][ 1 ], "WIFE") != true) && (strEqu(fieldElmntArr[ i ][ 1 ], "CHIL") != true)){
								
				GEDfield = malloc( sizeof(Field) );
				GEDfield -> tag = fieldElmntArr[ i ][ 1 ];
				
				if( fieldElmntArr[ i ][ 2 ] != NULL ){
					GEDfield -> value = fieldElmntArr[ i ][ 2 ];
				}else{
					GEDfield -> value = NULL;
				}
				
				insertBack( &GEDfam->otherFields, GEDfield );
				
			}
			

		}
		
		i ++;
		curLvl = atoi( fieldElmntArr[i][0]);

	}

	GEDlink -> linkStr = linkStr;
	GEDlink -> data = GEDfam;

	return GEDlink;

}

Link* parseIndiLink( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index ){
	
	Field* GEDfield;
	int curLvl = -1;
	char* curPath;
	int i = index;

	Link* GEDlink = malloc( sizeof(Link) );
	char* linkStr = malloc( sizeof(char) * 32 );
	Individual* GEDindi = createIndividual();
		
	while( i < size ){

		curPath = pathArr[i];

		if ( curLvl == 0 ){

			break;
            
		}else if ( fieldElmntArr[ i ][ 1 ][ 0 ] == '@' ){

			strcpy( linkStr, fieldElmntArr[ i ][ 1 ] );

		}else if ( strEqu( curPath, "INDI") ) {
            
            
		}else if( strEqu( curPath, "INDINAMEGIVN") ){

			/**
			 * { get GIVNen name information }
			 */

			GEDindi -> givenName = fieldElmntArr[ i ][ 2 ];

		} else if ( strEqu( curPath, "INDINAMESURN") ){

			/**
			 * { get NAME information }
			 */

			GEDindi -> surname = fieldElmntArr[ i ][ 2 ];

		} else if ( strEqu( curPath, "INDIEVEN") ) {

			/**
			 * { get EVENt information }
			 */

        } else if (strEqu(curPath, "INDIFAMC") || strEqu(curPath, "INDIFAMS")) {
            
			
//		} else if( (strEqu(curPath, "FAMC") != true && strEqu(curPath, "FAMS") != true) ||
		} else if ( strEqu(curPath, "INDINAME") ||
                    strEqu(curPath, "INDISEX ") ||
                    strEqu(curPath, "INDIBIRT") ||
                    strEqu(curPath, "INDITITL") ||
                    strEqu(curPath, "INDIBIRTDATE") ||
                    strEqu(curPath, "INDIBIRTPLAC") ||
                    strEqu(curPath, "INDIDEAT") ||
                    strEqu(curPath, "INDIDEATDATE") ||
                    strEqu(curPath, "INDIDEATPLAC") ||
                    strEqu(curPath, "INDIBURI") ||
                    strEqu(curPath, "INDIBURIDATE") ||
                    strEqu(curPath, "INDIBURIPLAC") ||
                    strEqu(curPath, "INDICHR ") ||
                    strEqu(curPath, "INDICHR DATE") ||
					
					
					strEqu(curPath, "INDIDATE") ||
					strEqu(curPath, "INDIPLAC") ||
					strEqu(curPath, "INDIDATE") ||
					strEqu(curPath, "INDIPLAC") ||
					strEqu(curPath, "INDIDATE") ||
					strEqu(curPath, "INDIPLAC") ||
					strEqu(curPath, "INDIDATE") ||
					strEqu(curPath, "INDIPLAC") ||
					
					
                    strEqu(curPath, "INDICHR PLAC")
                ) 
        {
				
            GEDfield = malloc( sizeof(Field) );
            GEDfield -> tag = fieldElmntArr[ i ][ 1 ];
            
			if( strEqu(curPath, "INDINAME") ){
				
				/* Construct SURN and GIVN from NAME. Will be over ridden by lower level  */
		
				char* fullName = strdup2(fieldElmntArr[ i ][ 2 ]);
				GEDindi->givenName = strtok(fullName, " ");
				
				char* surname = strdup2(strtok(NULL, "/"));
				
				if( surname != NULL ){
					
					surname[ strlen(surname) ] = '\0';
					GEDindi->surname = surname;
					
				}
				
			}else if( fieldElmntArr[ i ][ 2 ] != NULL ){
                GEDfield -> value = fieldElmntArr[ i ][ 2 ];
				 insertBack( &GEDindi->otherFields, GEDfield );
            }else{
                GEDfield -> value = NULL;
				 insertBack( &GEDindi->otherFields, GEDfield );
            }
           
                
		} else {
            /*
             * Unknown tag
             */
            createERROR(INV_RECORD, i);
            return NULL;
		}
		
		i ++;
		curLvl = atoi( fieldElmntArr[i][0]);

	}

	GEDlink -> linkStr = linkStr;
	GEDlink -> data = GEDindi;

	return GEDlink;

}

/* - - - - - - - - - - - - - - - - - - - - -*/
/* - - - SECOND PASS PARSING FUNCIONS - - - */
/* - - - - - - - - - - - - - - - - - - - - -*/

Family* parseFamily( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index, List* refList, Family* GEDfam ){

	int curLvl = -1;
	char* curPath;
	Link* ref;
	char* linkStr;
	int i = index;

	while( i < size ){

		curPath = pathArr[i];

		if( curLvl == 0 ){

			return GEDfam;

		}else if( strEqu( curPath, "FAM HUSB" ) ){

			/**
			 * { Get the pointer to the HUSBand }
			 */
			linkStr = fieldElmntArr[i][2];

			ListIterator iter = createIterator( *refList );

			void* elem;

			/**
			 * { Iterate through the list of Link + Data structures until we find match }
			 */

			while( (elem = nextElement(&iter)) != NULL){

				/**
				 * { when we do find it, dereference it and add the data to the HUSBand member }
				 */

				ref = (Link*)elem;
				

				if( strEqu( ref -> linkStr, linkStr ) ){
					
					GEDfam -> husband = (Individual*)(ref -> data);

				}

			}

		}else if ( strEqu( curPath, "FAM WIFE" ) ){

			/**
			 * { Get the pointer to the WIFE }
			 */

			linkStr = fieldElmntArr[i][2];

			ListIterator iter = createIterator( *refList );

			void* elem;

			/**
			 * { Iterate through the list of Link + Data structures until we find match }
			 */

			while( (elem = nextElement(&iter)) != NULL){
				/**
				 * { when we do find it, dereference it and add the data to the WIFE member }
				 */

				ref = (Link*)elem;

				if( strEqu( ref -> linkStr, linkStr ) == true ){
					GEDfam -> wife = (Individual*)(ref -> data);
				}
			}

		}else if ( strEqu( curPath, "FAM CHIL" ) ) {

			/**
			 * { Get the pointer to the WIFE }
			 */

			linkStr = fieldElmntArr[i][2];

			ListIterator iter = createIterator( *refList );

			void* elem;

			/**
			 * { Iterate through the list of Link + Data structures until we find match }
			 */

			while( (elem = nextElement(&iter)) != NULL){
				/**
				 * { when we do find it, dereference it and add the data to the WIFE member }
				 */
				ref = (Link*)elem;

				if( strEqu( ref -> linkStr, linkStr ) ){
					insertBack(&GEDfam -> children, (Individual*)(ref -> data));
				}
			}
			
		}
		
		i++;
		curLvl = atoi( fieldElmntArr[i][0]);

	}

	return GEDfam;
}

Individual* parseIndi( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index, List* refList, Individual* GEDindi ){

	Link* ref;
	char* linkStr;
	int curLvl = -1;
	char* curPath;
	int i = index;

	while( i < size ){

		curPath = pathArr[i];

		if ( curLvl == 0 ){

			return GEDindi;

		}else if( strEqu( curPath, "INDIFAMS") == true || strEqu( curPath, "INDIFAMC") == true){

			/**
			 * { Get the pointer to the FAMily }
			 */

			linkStr = fieldElmntArr[i][2];

			ListIterator iter = createIterator( *refList );

			void* elem;

			/**
			 * { Iterate through the list of Link + Data structures until we find match }
			 */

			while( (elem = nextElement(&iter)) != NULL){

				/**
				 * { when we do find it, dereference it and add the data to the family list member }
				 */

				ref = (Link*)elem;

				if( strEqu( ref -> linkStr, linkStr ) == true ){

					insertBack(&GEDindi -> families, (Family*)(ref -> data));

				}

			}
		
		}
		
		i ++;
		curLvl = atoi( fieldElmntArr[i][0]);

	}

	return GEDindi;
}

int endofline(FILE *fp, int c){
    
    int eol = (c == '\r' || c == '\n');
    if (c == '\r')
    {
        c = getc(fp);
        if (c != '\n' && c != EOF)
            ungetc(c, fp);
    }
    if (c == '\n')
    {
        c = getc(fp);
        if (c != '\r' && c != EOF)
            ungetc(c, fp);
    }

    return(eol);
}

int get_line(FILE *fp, char *buffer, size_t buflen){

    char *end = buffer + buflen - 1;
    char *dst = buffer;
    int c;
    while ((c = getc(fp)) != EOF && !endofline(fp, c) && dst < end)
        *dst++ = c;
    *dst = '\0';
    return((c == EOF && dst == buffer) ? EOF : dst - buffer);
}

Individual* dupIndi( const Individual* toDup ){

	Individual* new = createIndividual();
	new->givenName = strdup2( toDup->givenName );
	new->surname = strdup2( toDup->surname );
	return new;

}

void recurDescendants( const GEDCOMobject* familyRecord, List* descendants,  const Individual* indi ){

	ListIterator famIter = createIterator( familyRecord->families );
	void* famElem;
	Family* f;
	
	void* childElem;
	Individual* child;

	/* find ever direct descendant of INDI by iterating through every family */

	while( (famElem = nextElement(&famIter) ) != NULL){

		f = (Family*)famElem;

		if( f->husband == indi || f->wife == indi ){

			/* iterate through childern and add descendants recursivly */
			
			ListIterator childIter = createIterator( f->children );

			while( (childElem = nextElement(&childIter) ) != NULL){

				/* TODO: Make it so "child" points to a copy, not the OG */
				child = ( Individual* )childElem;
				insertBack( descendants, child );
				
				recurDescendants( familyRecord, descendants, child );

			}

		}

	}

	return;

}

void writeGEDCOMLine( char* lvl, char* tag, char* value, char** toAdd ){
	
	char* line = malloc( sizeof(char) * 1024 );
	strcpy( line, "" );
	strcat( line, lvl);
	strcat( line, " " );
	strcat( line, tag );
	
	if( value != NULL ){
		strcat( line, " " );
		strcat( line, value );
	}
	
	int lineLen = strlen( line );
	int toAddLen = strlen( *toAdd );
	
	*toAdd = realloc( *toAdd, sizeof(char) * lineLen + sizeof(char) * toAddLen + 10 ); 
	
	strcat( *toAdd, line );
	strcat( *toAdd, "\n" );
	
	return;
}

char* generateRef( char* lead, int seed ){
	
	/* changed 32 to 256. Might break? */
	char* ref = malloc( sizeof(char) * 256 );
	char* tmp = malloc( sizeof(char) * 256 );
	
	strcpy( ref, "@" );
	strcat( ref, lead );
	sprintf( tmp , "%03d", seed );
	strcat( ref, tmp );
	strcat( ref, "@" );
	
	return ref;
	
}

void safeStrCat( char* src, char** dest ){
	
	int srcLen = strlen( src );
	int destLen = strlen( *dest );
	
	*dest = realloc( *dest,  sizeof(char) * srcLen + sizeof(char) * destLen + 10 );
	strcat( *dest, src );
	
}

char* indiToGEDCOM( Individual* indi ){
	
	char* indiGEDCOM = malloc( sizeof(char) * 1024 );
	char* fullName = malloc( sizeof(char) * 1024 );
	
	strcpy( indiGEDCOM, "" );
	strcpy( fullName, "" );
	
	/* Generate full name for top level name field */
	
	if( indi->givenName != NULL ){
		strcat( fullName, indi->givenName );
		
	}
	
	
	if( indi->surname != NULL ){
		
		strcat( fullName, " /" );
		strcat( fullName, indi->surname );
		strcat( fullName, "/" );
	}
	
	
	int xref; /* use the memory location (int) of the pointer as a seed for an X-Ref */
	xref = (unsigned long long)indi;
	
	writeGEDCOMLine( "0", generateRef("I", xref), "INDI", &indiGEDCOM );
	writeGEDCOMLine( "1", "NAME", fullName , &indiGEDCOM );
	
	if( indi->givenName != NULL ){
		writeGEDCOMLine( "2", "GIVN", indi->givenName , &indiGEDCOM );
	}else{
		writeGEDCOMLine( "2", "GIVN", NULL , &indiGEDCOM );
	}
	
	if( indi->surname != NULL ){
		writeGEDCOMLine( "2", "SURN", indi->surname , &indiGEDCOM );
	}else{
		writeGEDCOMLine( "2", "SURN", NULL , &indiGEDCOM );
	}
	
	
	/* Generate FAM links */
	ListIterator iter = createIterator( indi->families );
	void* elem;
	Family* f;
	
	while( (elem = nextElement(&iter)) != NULL ){
		
		f = (Family*)elem;
		
		/* Check if this person is the spouse in a family */
		if( f->husband == indi ){
			
			xref = (unsigned long long)f;
			writeGEDCOMLine( "1", "FAMS", generateRef("F", xref) , &indiGEDCOM );
			
		}else if( f->wife == indi ){
			
			xref = (unsigned long long)f;
			writeGEDCOMLine( "1", "FAMS", generateRef("F", xref) , &indiGEDCOM );
			
		}else{
			
			/* Check if this person is the child in a family */
			ListIterator childIter = createIterator( f->children );
			void* childElem;
			Individual* child;
			
			while( (childElem = nextElement(&childIter)) != NULL ){
				
				child = (Individual*)childElem;
				
				if( child == indi ){
					
					xref = (unsigned long long)f;
					writeGEDCOMLine( "1", "FAMC", generateRef("F", xref) , &indiGEDCOM );
					break;
				}
				
			}
			
		}
		
	}
	
	/* Print other fields */
	iter = createIterator( indi->otherFields );
	char* otherGEDCOM = malloc( sizeof(char) * 1024 );
	strcpy( otherGEDCOM, "" );
	Field* g;
	
	while( (elem = nextElement(&iter)) != NULL ){
		g = (Field*)elem;
		
		if( (strEqu(g->tag, "GIVN") == false) && (strEqu(g->tag, "SURN") == false) && (strEqu(g->tag, "SURN") == false) ){
			
			otherGEDCOM =  otherToGEDCOM( g );
			safeStrCat( otherGEDCOM, &indiGEDCOM );
			
		}
		
	}
	 
	return indiGEDCOM;
}

char* famToGEDCOM( Family* fam ){
	
	char* famGEDCOM = malloc( sizeof(char) * 1024 );
	
	strcpy( famGEDCOM, "" );
	
	int xref; /* use the memory location (int) of the pointer as a seed for an X-Ref */
	xref = (unsigned long long)fam;
	
	writeGEDCOMLine( "0", generateRef("F", xref), "FAM" , &famGEDCOM );
	
	/* Print husband and wife */
	
	xref = (unsigned long long)fam->husband;
	writeGEDCOMLine( "1", "HUSB", generateRef("I", xref) , &famGEDCOM );
	
	xref = (unsigned long long)fam->wife;
	writeGEDCOMLine( "1", "WIFE", generateRef("I", xref) , &famGEDCOM );
	
	/* Generate FAM links */
	ListIterator iter = createIterator( fam->children );
	void* elem;
	Individual* child;
	
	while( (elem = nextElement(&iter)) != NULL ){
		
		child = (Individual*)elem;
		xref = (unsigned long long)child;
		writeGEDCOMLine( "1", "CHIL", generateRef("I", xref) , &famGEDCOM );
		
	}
	return famGEDCOM;
	
}

char* otherToGEDCOM( Field* g ){
	
	char* otherGEDCOM = malloc( sizeof(char) * 1024 );
	strcpy( otherGEDCOM, "" );
	
	writeGEDCOMLine( "1", g->tag, g->value , &otherGEDCOM );
	
	return otherGEDCOM;
	
}

char* printList( void* toBePrinted ){
	
	List l = *(List*)toBePrinted;
	return toString( l );
}

void recurDescendantsN( const GEDCOMobject* familyRecord, List* descendantsByGen,  const Individual* indi, unsigned int maxGen, int curGen ){
	
	if( curGen > maxGen && maxGen != 0){
		return;
	}
	
	List* genDescendants = NULL;

	ListIterator famIter = createIterator( familyRecord->families );
	void* famElem;
	Family* f;
	
	void* childElem;
	Individual* child;

	while( (famElem = nextElement(&famIter) ) != NULL){

		f = (Family*)famElem;

		if( f->husband == indi || f->wife == indi ){

			
			ListIterator childIter = createIterator( f->children );

			while( (childElem = nextElement(&childIter) ) != NULL){

				child = ( Individual* )childElem;
				
				/* Before inserting, we need to ensure this INDI is not already listed in the generation */
				
				if( genDescendants == NULL ){
					
					ListIterator iter = createIterator( *descendantsByGen );
					void* elem = nextElement( &iter );
					
					for( int o = 1; o < curGen; o ++ ){
						
						if( elem == NULL ){ break; }
						elem = nextElement( &iter );
						
					}
					
					if( elem == NULL ){
						genDescendants = malloc( sizeof(List) );
						*genDescendants = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
						insertBack( descendantsByGen, genDescendants );
					}else{
						genDescendants = (List*)elem;
					}
					
				}
				
				bool notInList = true;
				ListIterator subIter = createIterator( *genDescendants );
				void* subElem;
				Individual* subToken;
				
				while( (subElem = nextElement(&subIter) ) != NULL){
					
					subToken = (Individual*)subElem;
					if( subToken == child  ){
						notInList = false;
					}
					
				}
				
				if( notInList == true ){
					
					
					insertSorted( genDescendants, child );
				}
				
				recurDescendantsN( familyRecord, descendantsByGen, child, maxGen, curGen + 1 );
			}
		}
	}
	
	return;
}



void recurAncestorsN( const GEDCOMobject* familyRecord, List* ancestorsByGen,  const Individual* indi, unsigned int maxGen, int curGen ){
	
	if( curGen > maxGen && maxGen != 0){
		return;
	}
	
	List* genAncestors = NULL;

	ListIterator famIter = createIterator( familyRecord->families );
	void* famElem;
	
	Individual* child;
	Family* f;
	
	/* Loop through all families in FAMILY RECORD */
	while( (famElem = nextElement(&famIter) ) != NULL){
		
		/* F is the current family */
		f = (Family*)famElem;
		
		
		/* For F, iterate through and determine if INDI is a child or not*/
		ListIterator subFamIter = createIterator( f->children );
		void* subFamElem;
		bool isChild = false;
		
		while( (subFamElem = nextElement(&subFamIter) ) != NULL){
			
			child = (Individual*)subFamElem;
			if( child == indi ){
				isChild = true;
				break;
			}
			
		}
		
		/* If INDI is a child in F, add the family's parents as ancestors */
		if( isChild ){
			
			/* If the family has a father */
			if( f->husband != NULL ){
				
				/* If this generation is NULL, we need to create it / find it */
				if( genAncestors == NULL ){
					
					
					ListIterator iter = createIterator( *ancestorsByGen );
					void* elem = nextElement( &iter );
					
					
					/* Iterate to the current generation  */
					for( int o = 1; o < curGen; o ++ ){
						
						if( elem == NULL ){ break; }
						elem = nextElement( &iter );
						
					}
					
					/* If there currently is not generation list, make it */
					if( elem == NULL ){
						genAncestors = malloc( sizeof(List) );
						*genAncestors = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
						insertBack( ancestorsByGen, genAncestors );
					}else{
						genAncestors = (List*)elem;
					}
					
					
					
				}
				
				insertSorted( genAncestors, f->husband );
				
				/* Go find the parents of the husband and add them to their own generation list */
				recurAncestorsN( familyRecord, ancestorsByGen, f->husband, maxGen, curGen + 1 );
				
			}
			
			if( f->wife != NULL ){
				
				if( genAncestors == NULL ){
					
					ListIterator iter = createIterator( *ancestorsByGen );
					void* elem = nextElement( &iter );
					
					for( int o = 1; o < curGen; o ++ ){
						
						if( elem == NULL ){ break; }
						elem = nextElement( &iter );
						
					}
					
					if( elem == NULL ){
						genAncestors = malloc( sizeof(List) );
						*genAncestors = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
						insertBack( ancestorsByGen, genAncestors );
					}else{
						genAncestors = (List*)elem;
					}
					
				}
				
				insertSorted( genAncestors, f->wife );
				recurAncestorsN( familyRecord, ancestorsByGen, f->wife, maxGen, curGen + 1 );
			}
		}
	}
	return;
}

int getIntTest( char c ){

	return (int)c;

}
/*
Individual* individualDup( Individual* toCopy ){
	
	Individual* indi = createIndividual();
	ListIterator iter = createIterator( toCopy->families );
	void* elem;
	Family* fam;
	while( (elem = nextElement(&iter) ) != NULL){
		
		fam = (Family*)elem;
		insertBack(&indi->families, fam);
		
	}
	
	iter = createIterator( toCopy->otherFields );
	Field* other;
	while( (elem = nextElement(&iter) ) != NULL){
		
		other = (Field*)elem;
		insertBack(&indi->otherFields, other);
		
	}
	
	if( toCopy->surname != NULL ){
		indi->surname = strdup( toCopy->surname );
	}
	
	if( toCopy->givenName != NULL ){
		indi->givenName = strdup( toCopy->givenName );
	}
	
	return indi;
}

List individualListDup( List* toCopy ){
	
	List dupList = initializeList( &printIndividual, &FreeIndividual, &compareIndividuals );
	
	ListIterator iter = createIterator( *toCopy );
	void* elem;
	Individual* token;
	Individual* dup;
	
	while( (elem = nextElement(&iter) ) != NULL){
		
		token = (Individual*)elem;
		dup = individualDup( token );
		insertBack( &dupList , dup );
		
	}
	
	return dupList;
	
}

List genListDup( List* toCopy ){
	
	List dupList = initializeList( &printList, &noFree, &compareList );
	
	ListIterator iter = createIterator( *toCopy );
	void* elem;
	List* token;
	List* dup = NULL;
	
	while( (elem = nextElement(&iter) ) != NULL){
		
		token = (List*)elem;
		dup = &individualListDup( token ); //This line / area giving problems
		insertBack( &dupList , dup );
		
	}
	
	return dupList;
	
}
*/
