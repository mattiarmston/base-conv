#include <argp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void makeLowercase(char[]);
int intOfString(char[]);

// Argp declarations
const char* argp_program_version = "base-conv 1.0.1";
const char* argp_program_bug_address = "matti.armston@gmail.com";
static char doc[] = "Converts a number from base 2-36 to base 2-36";
static char args_doc[] = "NUMBER BASE_FROM BASE_TO";
static struct argp_option options[] = {
  {"output-file", 'o', "FILE", 0, "Write output to FILE"},
  { 0 }
};
struct arguments {
  char* args[3];
  char* filename;
};
static error_t parse_opt(int key, char* arg, struct argp_state* state) {
  struct arguments* arguments = state->input;
  switch (key) {
    case 'o':
      arguments->filename = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 3) {
        argp_usage(state);
      }
      arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 3) {
        argp_usage(state);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
static struct argp argp = {options, parse_opt, args_doc, doc};

int baseFromString(char string[]) {
  int base = atoi(string);
  if (base != 0) {
    return base;
  }
  // atoi returns 0 as an 'error' message, if this happens,
  // it is dealt with below
  makeLowercase(string);
  int intString = intOfString(string);
  switch(intString) {
    case 719: case 300: // decimal dec
      return 10;
    case 1141: case 325: // hexadecimal hex
      return 16;
    case 645: case 313: // binary bin
      return 2;
    case 531: case 326: // octal oct
      return 8;
    default: // error value
      return -1;
  }
}

int intOfString(char string[]) {
  int intString = 0;
  char* pString = string; int length = strlen(pString);
  // sums the ascii values of all the characters in a string
  // this isn't perfect but is good enough for our needs
  for (int i = 0; i < length; i++) {
    int intChar = string[i];
    intString += intChar;
  }
  return intString;
}

void makeLowercase(char string[]) {
  char* pString = string; int length = strlen(pString);
  for (int i = 0; i < length; i++) {
    string[i] = tolower(string[i]);
  }
}

int intFromChar(char* digit) {
  if (*digit == 48) {
    return 0;
  }
  int val = atoi(digit);
  if (val != 0) {
    return val;
  }
  // atoi returns 0 as an 'error' message
  // if 0 is returned, then we likely entered a letter
  // so it is dealt with below
  int asciiVal = tolower(*digit);
  if ((asciiVal < 97) || (asciiVal > 122)) {
    // the ascii values of all lowercase letters are between 97 and 122
    // if asciiVal is not in between these values,
    // then we have likely inputed a symbol
    return -1; // error value
  }
  // the ascii value of 'a' is 97, 'b' is 98...
  // the numerical value of 'a' is 10, 'b' is 11...
  // therefore to get from the ascii value to the numerical value
  // we do aciiVal - 87
  return asciiVal - 87;
}

int validateStrNum(char* strNum, int base) {
  int length = strlen(strNum);
  for (int i = 0; i < length; i++) {
    char charDigit = strNum[i];
    int digit = intFromChar(&charDigit);
    if (digit == -1) {
      printf("Error: invalid character '%c', must be a number or letter\n",
          charDigit);
      return 0; // invalid number
    }
    if (digit >= base) {
      printf("Error: invalid number, '%c' is not a valid base %d digit\n",
          charDigit, base);
      return 0; // invalid number
    }
  }
  return 1; // valid number
}

// Return the base 10 representation of `strNum`.
// `strNum` is string representation of a base `base` number.
int getStrNumVal(char* strNum, int base) {
  char* pStrNum = strNum; int strNumLen = strlen(pStrNum);
  int numVal = 0;
  for (int i = 0; i < strNumLen; i++) {
    // if strNum[i] is used, the entire array from i to end is converted
    // this ensures that only 1 digit is converted at a time
    char charDigit = strNum[i];
    int digit = intFromChar(&charDigit);
    // gets the value of the current column e.g. ones, tens, hundreds etc.
    // for the current base
    int columnVal = pow(base, strNumLen - i - 1);
    int digitVal = digit * columnVal;
    numVal += digitVal;
  }
  return numVal;
}

// Return a base `baseTo` string representation of the base 10 number `inputNum`.
char* outNumFrominNum(int inputNum, int baseTo) {
  // The column currently being dealt with.  E.g., units, tens, hundreds, etc.,
  // but in the appropriate base.
  unsigned long columnVal;
  // The magnitued of the number.  E.g., 100 in base 10 has 3 digits, so it has
  // magnitude of 3.
  unsigned int magnitude;
  // Initialize columnVal and magnitude.
  for (magnitude = 1, columnVal = baseTo; inputNum >= columnVal; magnitude++) {
    columnVal *= baseTo;
  }
  columnVal /= baseTo;
  // Create zero'd char array big enought to hold output string.
  size_t outputNumSize = sizeof(char) * (magnitude + 1);
  char* outputNum = malloc(outputNumSize);
  memset(outputNum, 0, outputNumSize); // this also adds a null byte to the end
  // calcultate string representation of number
  for (int i = 0; i < magnitude; i++) {
    // this is integer division
    int digit = inputNum / columnVal;
    if (digit < 10) {
      // the ascii value of '0' is 48, '1' is 49...
      // the numerical value of '0' is 0, '1' is 1...
      // so to get from the numerical value to the ascii value, we do digit + 48
      char charDigit = digit + 48;
      outputNum[i] = charDigit;
    } else {
      // the ascii value of 'A' is 65, 'B' is 66...
      // the numerical value of 'A' is 10, 'B' is 11...
      // so to get from the numerical value to the ascii value, we do digit + 55
      char charDigit = digit + 55;
      outputNum[i] = charDigit;
    }
    int remainder = inputNum - digit * columnVal;
    inputNum = remainder;
    // reduce columnVal to represent new column e.g. ones, tens, hundreds etc.
    // for the correct base
    columnVal /= baseTo;
  }
  return outputNum;
}

void output(char* message, FILE* pFile) {
  if (pFile) {
    fprintf(pFile, "%s\n", message);
  } else {
    // a null value of pFile means output to stdout
    printf("%s\n", message);
  }
}

int main(int argc, char** argv) {
  // parse arguments
  struct arguments args;
  args.filename = NULL;
  argp_parse(&argp, argc, argv, 0, 0, &args);
  char* strNum = args.args[0];
  int baseFrom = baseFromString(args.args[1]);
  int baseTo = baseFromString(args.args[2]);
  // guard clauses to prevent invalid base values
  if ((baseFrom == -1) || (baseTo == -1)) {
    printf("Error: invalid base, try using a numeric value instead\n");
    return 1;
  }
  if ((baseFrom > 36) || (baseTo > 36)) {
    printf("Error: cannot have a base higher than 36\n");
    return 1;
  }
  if ((baseFrom < 2) || (baseTo < 2)) {
    printf("Error: cannot have a base lower than 2\n");
    return 1;
  }
  // file handling
  // NULL means no file, write to stdout
  FILE* pFile = NULL;
  if (args.filename) {
    pFile = fopen(args.filename, "a");
    if (!pFile) {
      printf("Error: couldn't open file, writing to stdout");
    }
  }
  // validate input
  if (!validateStrNum(strNum, baseFrom)) {
    // validateStrNum prints error messages, but doesn't exit
    return 1;
  }
  // get base 10 representation of input number
  int inputNum = getStrNumVal(strNum, baseFrom);
  // get base X string representation of input number
  char* outputStr = outNumFrominNum(inputNum, baseTo);
  output(outputStr, pFile);
  // cleanup
  free(outputStr);
  if (pFile) {
    fclose(pFile);
  }
  return 0;
}
