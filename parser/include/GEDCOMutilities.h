#ifndef GEDCOMUTILITIES_H
#define GEDCOMUTILITIES_H

typedef struct linkStruct{

	char* linkStr;
	void* data;

}Link;

typedef struct famiyRefStruct{

	char* linkStr;
	Family* data;

}famLink;

typedef struct individualRefStruct{

	char* linkStr;
	Individual* data;

}indiLink;

GEDCOMerror createERROR(ErrorCode type, int line);

Header* createHeader( );

char* strdup2(const char * s);
char* strdup(const char * s);

bool strEqu(char * a, char * b);

void deleteLink(void* toBeDeleted);

int compareLink(const void* first, const void* second);

char* printLink(void* toBePrinted);

char* printIndividualNested(void* toBePrinted);

char* printFieldNested(void* toBePrinted);

char* printSubmitter(void* toBePrinted);

void deleteHeader(void* toBeDeleted);

void deleteSubmitter(void* toBeDeleted);

void noFree(void* toBeDeleted);

void FreeLinkStr(void* toBeDeleted);

void FreeIndividual(void* toBeDeleted);

void FreeFamily(void* toBeDeleted);

char* genPathList( char** fieldElmntArr[], int size );

Submitter* parseSubmitter( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index );

Header* parseHeader( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index );

Individual* parseIndi( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index, List* refList, Individual* GEDindi  );

Family* parseFamily( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index, List* refList,  Family* GEDfam );

Link* parseFamilyLink( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index );

Link* parseIndiLink( char pathArr[][ 64 ], char** fieldElmntArr[], int size, int index );

int get_line(FILE *fp, char *buffer, size_t buflen);

int endofline(FILE *fp, int c);

void recurDescendants( const GEDCOMobject* familyRecord, List* descendants, const Individual* indi );

void writeGEDCOMLine( char* lvl, char* tag, char* value, char** toAdd );

char* generateRef( char* lead, int seed );

char* indiToGEDCOM( Individual* indi );

char* famToGEDCOM( Family* fam );

char* otherToGEDCOM( Field* g );

void safeStrCat( char* src, char** dest );

void recurDescendantsN( const GEDCOMobject* familyRecord, List* descendantsByGen,  const Individual* indi, unsigned int maxGen, int curGen );

char* printList( void* toBePrinted );

int compareList(const void* first,const void* second);

void recurAncestorsN( const GEDCOMobject* familyRecord, List* ancestorsByGen,  const Individual* indi, unsigned int maxGen, int curGen );

Individual* createIndividual();

Submitter* createSubmitter( size_t len );

List genListDup( List* toCopy );
#endif
