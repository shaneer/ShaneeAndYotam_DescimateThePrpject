#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPLogger.h"

struct sp_config_t{
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix;
	int spNumOfImages;
	int spPCADimension;
	char* spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char* spLoggerFilename;
};

//Check the paramNames to see if the name fits any and if so assign proper value
int loadData(SPConfig res, char* paramName, char* value, const char* filename, int lineNum, SP_CONFIG_MSG* msg){
	//TEMPORARY VALUE FOR VALIDITY CHECKS
	int tempVal;
	char* tempStr;

	if (strcmp(paramName, "spImagesDirectory")==0) {
		tempStr = readStr(value, filename, lineNum, msg);
		strcpy(res->spImagesDirectory, tempStr);
		if (res->spImagesDirectory == NULL){
			return 4;
		}
		return 0;
	}
	else if (strcmp(paramName, "spImagesPrefix")==0) {
		tempStr = readStr(value, filename, lineNum, msg);
		strcpy(res->spImagesPrefix , tempStr);
		if (res->spImagesPrefix == NULL){
			return 4;
		}
		return 1;
	}
	else if (strcmp(paramName, "spImagesSuffix")==0) {
		tempStr = readSuffix(value, filename, lineNum, msg);
		strcpy(res->spImagesSuffix, tempStr);
		if (res->spImagesSuffix == NULL){
			return 4;
		}
		return 2;
	}
	else if (strcmp(paramName, "spNumOfImages")==0) {
		tempVal = readInt(value, INT_MAX, 0, filename, lineNum, msg, 4);
		if (tempVal == -1){
			return 4;
		}
		res->spNumOfImages = tempVal;
		return 3;
	}
	else if (strcmp(paramName, "spPCADimension")==0) {
		tempVal = readInt(value, 28, 10, filename, lineNum, msg, 5);
		if (tempVal == -1){
			return 4;
		}
		res->spPCADimension = tempVal;
	}
	else if (strcmp(paramName, "spPCAFilename")==0) {
		tempStr = readStr(value, filename, lineNum, msg);
		strcpy(res->spPCAFilename, tempStr);
		if (res->spPCAFilename == NULL){
			return 4;
		}
	}
	else if (strcmp(paramName, "spNumOfFeatures")==0) {
		tempVal = readInt(value, INT_MAX, 0, filename, lineNum, msg, 7);
		if (tempVal == -1){
			return 4;
		}
		res->spNumOfFeatures = tempVal;
	}
	else if (strcmp(paramName, "spExtractionMode")==0) {
		res->spExtractionMode = readBool(value, filename, lineNum, msg);
		if (res->spExtractionMode == -1){
			return 4;
		}
	}
	else if (strcmp(paramName, "spNumOfSimilarImages")==0) {
		tempVal = readInt(value, INT_MAX, 1, filename, lineNum, msg, 9);
		if (tempVal == -1){
			return 4;
		}
		res->spNumOfSimilarImages = tempVal;
	}
	else if (strcmp(paramName, "spKDTreeSplitMethod")==0) {
		res->spKDTreeSplitMethod = readEnum(value, filename, lineNum, msg);
		if (res->spKDTreeSplitMethod == INVALID){
			return 4;
		}
	}
	else if (strcmp(paramName, "spKNN")==0) {
		tempVal = readInt(value, INT_MAX, 1, filename, lineNum, msg, 11);
		if (tempVal == -1){
			return 4;
		}
		res->spKNN = tempVal;
	}
	else if (strcmp(paramName, "spMinimalGUI")==0) {
		res->spMinimalGUI = readBool(value, filename, lineNum, msg);
		if (res->spMinimalGUI == -1){
			return 4;
		}
	}
	else if (strcmp(paramName, "spLoggerLevel")==0) {
		tempVal = readInt(value, -1, 1, filename, lineNum, msg, 13);
		if (tempVal == -1){
			return 4;
		}
		res->spLoggerLevel = tempVal;
	}
	else if (strcmp(paramName, "spLoggerFilename")==0) {
		tempStr = readStr(value, filename, lineNum, msg);
		strcpy(res->spLoggerFilename, tempStr);
		if (res->spLoggerFilename == NULL){
			return 4;
		}
	}
	else {
	*msg = SP_CONFIG_INVALID_LINE;
	return 4;
	}
	return 5;
}

//We use a boolean array to ensure all our non-defaults are set
int checkNonDefaultsSet(bool chechValid[6], SP_CONFIG_MSG* msg, const char* filename, int lineNum){

	if (chechValid[0] == false){
		printParamNotSet(filename, lineNum, "spImagesDirectory");
		*msg = SP_CONFIG_MISSING_DIR;
		return -1;
	}
	else if (chechValid[1] == false){
		printParamNotSet(filename, lineNum, "spImagesPrefix");
		*msg = SP_CONFIG_MISSING_PREFIX;
		return -1;
	}
	else if (chechValid[2] == false){
		printParamNotSet(filename, lineNum, "spImagesSuffix");
		*msg = SP_CONFIG_MISSING_SUFFIX;
		return -1;
	}
	else if ( chechValid[3] == false){
		printParamNotSet(filename, lineNum, "spNumOfImages");
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		return -1;
	}
	return 0;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
	assert( msg != NULL );
	FILE *fp;
	SPConfig res;
	char* temp;
	char* currLine;
	char* tempVal;
	char* value;
	char* paramName;
	int lineNum;
	int checkValidLoad;
	bool checkValid[6] = {false, false, false, false, true, true};  //First four ar enon-defaults, fifth checks for errors and 6th is irrelevant toggle

	if (filename == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
	}

	res = (SPConfig) malloc(sizeof(*res));
	if (res == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	temp = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	value = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	paramName = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	res->spPCAFilename = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	res->spLoggerFilename = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	res->spImagesSuffix = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	res->spImagesPrefix = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);
	res->spImagesDirectory = (char*) malloc(CONFIG_LINE_MAX_SIZE+1);

		if (temp == NULL || value==NULL || paramName==NULL ||  !res->spPCAFilename || !res->spLoggerFilename
			|| !res->spImagesSuffix || !res->spImagesPrefix || !res->spImagesDirectory) {
			free(value);
			free(temp);
			free(paramName);
			spConfigDestroy(res);
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}//All memory allocations were succesfful

	if((fp = fopen(filename, "r")) == NULL){
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		free(value);
		free(temp);
		free(paramName);
		spConfigDestroy(res);
		if (strcmp(filename, "spcbir.config")==0){			//Tells us program is using default config, and it's faulty
			printf("The default config file spcbir is faulty");
		}
		return NULL;
	}
	//We have now succesfully allocated memory for our config struct res, opened our config_file
	// and allocated memory for temp. We begin by setting all the defaults.
	setDefaults(res);
	//We now have a config object with all defaults set, we initialize line counter and begin to review file.
	lineNum = 0;
	//ASSUME succes
	*msg = SP_CONFIG_SUCCESS;
	while(fgets(temp, CONFIG_LINE_MAX_SIZE, fp) != NULL && feof(fp) == 0  && *msg == SP_CONFIG_SUCCESS && checkValid[4]==true) {
				lineNum++;															//Count starts at 1
				//Skips over spaces,  comments and newlines
				currLine = temp;
				int ind=0;
				while (isspace(*currLine)){
					++currLine;
				}
				if (*currLine == '#'||*currLine =='\n'||*currLine =='\0') {		//Skips comments 'newline's and emptylines
					continue;
				}
				//First we copy the part after the '=' to value
				tempVal = strchr(temp, '=');

				//Now copy param name having already skipped preceeding whitespace, until first space or until '='
				while ( !isspace(currLine[ind]) && currLine[ind]!= '=' ){
						paramName[ind] = currLine[ind];
						ind++;
				}
				paramName[ind]='\0';
				//We now have our paramNme  and need only to obtain value string by 'cleaning' the string
				++tempVal;
				while (isspace(*tempVal)){
					++tempVal;
				}
				int indV = 0;
				while (!isspace(tempVal[indV]) && tempVal != '\0'){
					value[indV] = tempVal[indV];
					indV++;
				}
				value[indV]='\0';

				//FUNCTION THAT LOADS DATA INTO PARAM NAMES RETURNING A INDEX IN OUR BOOL ARRAY TO CHANGE
				checkValidLoad = loadData(res, paramName, value, filename, lineNum, msg);
				checkValid[checkValidLoad] = (checkValid[checkValidLoad]+1)%2;
 		}//END OF WHILE LOOP

 checkValidLoad = checkNonDefaultsSet(checkValid, msg, filename, lineNum);
 if (!*msg == SP_CONFIG_SUCCESS || checkValidLoad<0){
	 spConfigDestroy(res);
	 res = NULL;
 }
  free(value);
	free(temp);
	free(paramName);
	fclose(fp);
	return res;
}


//PRINTING ERROR MESSAGES TO CONSOLE
void printConstraintsNotMet(const char* filename, int numOfValue){
	printf("File: %s\nLine: %d\nMessage: Invalid value - constraint not met",filename, numOfValue );
}
void printInvalidLine(const char* filename, int lineNum){
	printf("File: %s\nLine: %d\nMessage: Invalid configuration line",filename, lineNum );
}
//TODO
void printParamNotSet(const char* filename, int lineNum, char* paramName){
	printf("File: %s\nLine: %d\nMessage: Parameter %s is not set",filename, lineNum, paramName );
}

//HELPER FUNCTIONS TO LOAD DATA FROM STRING FORN IN CONFIG LINE
void setDefaults(SPConfig config){

	config->spPCADimension = 20;
	strcpy(config->spPCAFilename, "pca.yml");
	config->spNumOfFeatures = 100;
	config->spExtractionMode = 1;
	config->spNumOfSimilarImages = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spKNN = 1;
	config->spMinimalGUI = 0;
	config->spLoggerLevel = 3;
	strcpy(config->spLoggerFilename, "stdout");

	return;
	}

//Helper Function to read int from config line
int readInt(char* value, int maxLength, int minLength, const char* filename, int lineNum, SP_CONFIG_MSG* msg, int NumOfValue){
	int num;
	if (isValidInt(value)){
		num = atoi(value);
	}else{
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	if( minLength <= num && num<=maxLength ){
		return num;
	}else {
		printConstraintsNotMet(filename, NumOfValue);
		*msg = SP_CONFIG_INVALID_INTEGER;
		return -1;
	}
}

//Helper Function to read and check string assignmint from config line
char* readStr(char* val, const char* filename, int lineNum, SP_CONFIG_MSG* msg){
	if (val == NULL){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_STRING;
		return NULL;
	}
	if (isValidString(val)){
		return val;
	}else{
		printInvalidLine(filename, lineNum);
		*msg =SP_CONFIG_INVALID_STRING;
		return NULL;
	}
}

//Helper Function to read and check string assignmint from config line
char* readSuffix(char* val, const char* filename, int lineNum, SP_CONFIG_MSG* msg){
	if (val == NULL){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_STRING;
		return NULL;
	}
	if (!isValidString(val)){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_STRING;
		return NULL;
	}
	if ( (strcmp(val,".png")==0) || (strcmp(val,".bmp")==0) || (strcmp(val,".jpg")==0) || (strcmp(val,".gif")==0) ){
		return val;
	}
	printInvalidLine(filename, lineNum);
	*msg =  SP_CONFIG_INVALID_STRING;
	return NULL;
}

//Helper Function to read  boolean assignmint from config line
bool readBool(char* val, const char* filename, int lineNum, SP_CONFIG_MSG* msg){				//TODO - can the user define bool as 0/1 does case matter True/true/tRue??
	if (val == NULL){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	if (!isValidString(val)){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_LINE;
		return -1;
	}
	if ( strcmp(val,"true")==0 ){
		return true;
	}
	else if ( strcmp(val,"true")==0 ){
		return false;
	}
	printInvalidLine(filename, lineNum);
	*msg = SP_CONFIG_INVALID_STRING;
	return -1;
}

//Helper Function to read and check enum value assignmint from config line
SP_SPLIT_METHOD readEnum(char* val, const char* filename, int lineNum, SP_CONFIG_MSG* msg){
	if (val == NULL){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return INVALID;
	}
	if (!isValidString(val)){
		printInvalidLine(filename, lineNum);
		*msg = SP_CONFIG_INVALID_STRING;
		return INVALID;
	}
	SP_SPLIT_METHOD method;
	if (strcmp(val,"MAX_SPREAD")==0){
		method = MAX_SPREAD;
		return method;
	}
	else if (strcmp(val,"RANDOM")==0){
		method = RANDOM;
		return method;
	}
	else if (strcmp(val,"INCREMENTAL")==0){
		method = INCREMENTAL;
		return method;
	}
	printInvalidLine(filename, lineNum);
	*msg = SP_CONFIG_INVALID_LINE;
	return INVALID;
}

// Helper functino to check validity of a string we want to use as an int value
bool isValidInt(char *str){
   // Handle empty string or negative "-"
   if (!*str || *str == '-'){
      return false;
		}
   //Check for non-digit chars in the rest of the string
   while (*str){
      if (!isdigit(*str)){
         return false;
      }
      else
         ++str;
   }
   return true;
}

// Helper functino to check validity of a string we want to use as a string value to be assigned
bool isValidString(char *str){
   if (!*str){
      return false;
	}
   //Check for any spaces
   while (*str){
      if (isspace(*str) || (*str)=='#'){
         return false;
			 }
			 else{
         ++str;
   		}
	}
	return true;
}

//GETTERS FOR FIELDS OF CONFIG STRUCT
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	assert( msg != NULL );
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	assert( msg != NULL );
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;

}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
	assert( msg != NULL );
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	int num = config->spNumOfImages;
	*msg = SP_CONFIG_SUCCESS;

	return num;
}

//TODO
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
	assert( msg != NULL );
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	int num = config->spNumOfFeatures;
  *msg = SP_CONFIG_SUCCESS;

	return num;
}
//TODO
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
	assert( msg != NULL );
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	int num = config->spPCADimension;
	*msg = SP_CONFIG_SUCCESS;

	return num;
}


char* spConfigGetImageDirectory(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if (config->spImagesDirectory == NULL){
		*msg = SP_CONFIG_MISSING_DIR;
		return NULL;
	}
	return config->spImagesDirectory;
}

//TODO
char* spConfigGetPrefix(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if (config->spImagesPrefix == NULL){
		*msg = SP_CONFIG_MISSING_DIR;
		return NULL;
	}
	return config->spImagesPrefix;
}

//TODO
char* spConfigGetSuffix(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if (config->spImagesSuffix == NULL){
		*msg = SP_CONFIG_MISSING_DIR;
		return NULL;
	}
	return config->spImagesSuffix;
}

char* spConfigGetPCAFilename(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if (config->spPCAFilename == NULL){
		*msg = SP_CONFIG_MISSING_DIR;
		return NULL;
	}
	return config->spPCAFilename;
}
//END OF GETTERS

// Function to return the fullImage path of a given index , combinig and returning a fromatted string of a few fields, and the index.
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index){
	SP_CONFIG_MSG msg;
	if (config == NULL){
		msg = SP_CONFIG_INVALID_ARGUMENT;
		return msg;
	}
	sprintf(imagePath, "%s%s%d%s", spConfigGetImageDirectory(config, &msg), spConfigGetPrefix(config, &msg), index, spConfigGetSuffix(config, &msg));
	msg = SP_CONFIG_SUCCESS;
	return msg;
 }

// Function to return the full PCA filename , combinig and returning a fromatted string of two fields of our struct.
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
	SP_CONFIG_MSG msg;
	if (config == NULL){
		msg = SP_CONFIG_INVALID_ARGUMENT;
		return msg;
	}
	sprintf(pcaPath, "%s%s", spConfigGetImageDirectory(config, &msg), spConfigGetPCAFilename(config, &msg));
	msg = SP_CONFIG_SUCCESS;
	return msg;
}

/*Function to destroy a config object
*
* Frees all previously allocated members of the struct as well as the pointer to the struct itself.
* @param -  config : struct to be destroyed
*
*/
void spConfigDestroy(SPConfig config){
	if (!config){
		return;
	}
	free(config->spImagesDirectory);
	free(config->spImagesPrefix);
	free(config->spImagesSuffix);
	free(config->spPCAFilename);
	free(config->spLoggerFilename);
	//All other members are Integer values (int, bool, enum) and do not require the use of free()
	free(config);
	return;
}
