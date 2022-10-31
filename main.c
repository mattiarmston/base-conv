#include <argp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Argp declarations
const char* argp_program_version = "base-converter 1.0.0";
const char* argp_program_bug_address = "matti.armston@gmail.com";
static char doc[] = "Converts between hexadecimal and decimal numbers";
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

void output(char* message, FILE* pFile) {
  if (pFile) {
    fprintf(pFile, "%s\n", message);
  } else {
    printf("%s\n", message);
  }
}

int intOfString(char string[]) {
  int intString = 0, size = 0;
  for (char* pString = string; *pString != 0; pString++) { size ++; }
  for (int i = 0; i < size; i++) {
    int intChar = string[i];
    intString += intChar;
  }
  return intString;
}

void makeLowercase(char string[]) {
  int size = 0;
  for (char* pString = string; *pString != 0; pString++) { size ++; }
  for (int i = 0; i < size; i++) {
    string[i] = tolower(string[i]);
  }

}

int baseFromString(char string[]) {
  int base = atoi(string);
  if (base != 0) {
    return base;
  }
  makeLowercase(string);
  int intString = intOfString(string);
  switch(intString) {
    case 719: case 300: // decimal dec
      return 10;
    case 1141: case 325: // hexadecimal hex
      return 16;
    case 645: case 313: // binary bin
      return 2;
    case 531: case 326:
      return 8;
    default:
      return -1;
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
  int asciiVal = tolower(*digit);
  return asciiVal - 87;

  // This won't ever be called
  // It is safer but also slower to write and I can't be bothered to
  switch(tolower(*digit)) {
    case 'a':
      return 10;
    case 'b':
      return 11;
    case 'c':
      return 12;
    case 'd':
      return 13;
    case 'e':
      return 14;
    case 'f':
      return 15;
    default:
      return val; // This will be zero
  }
}

int getStrNumVal(char* strNum, int base) {
  int strNumSize = 0;
  for (char* pStrNum = strNum; *pStrNum != 0; pStrNum++) { strNumSize ++; }
  int numVal = 0;
  for (int i = 0; i < strNumSize; i++) {
    char charDigit[1]; charDigit[0] = strNum[i]; // Won't convert entire array at once
    int digit = intFromChar(charDigit);
    int columnVal = pow(base, strNumSize - i - 1);
    int digitVal = digit * columnVal;
    if (digitVal > columnVal * (base - 1)) {
      printf("Error: invalid number, digit %d is larger than, or equal to, BASE_FROM\n",
        i + 1);
      exit(1);
    }
    numVal += digitVal;
  }
  return numVal;
}

struct charArray {
  int noElements;
  char* pArray;
};

struct charArray outNumFrominNum(int inputNum, int baseTo) {
  int maxColumnVal = 1 * baseTo;
  int noDigits = 0;
  for (noDigits = 1; inputNum >= maxColumnVal; noDigits++) { maxColumnVal *= baseTo; }
  maxColumnVal /= baseTo;
  int outputNumSize = sizeof(char) * (noDigits + 1);
  char* outputNum = malloc(outputNumSize);
  memset(outputNum, 0, outputNumSize);
  for (int i = 0; i < noDigits; i++) {
    int digit = inputNum / maxColumnVal;
    if (digit + 48 < 58) {
      char charDigit = digit + 48;
      outputNum[i] = charDigit;
    } else {
      char charDigit = digit + 55;
      outputNum[i] = charDigit;
    }
    int remainder = inputNum - digit * maxColumnVal;
    inputNum = remainder;
    maxColumnVal /= baseTo;
  }
  outputNum[noDigits] = 0;
  struct charArray outputArray = {noDigits + 1, outputNum};
  return outputArray;
}

int main(int argc, char** argv) {
  struct arguments args;
  args.filename = NULL;
  argp_parse(&argp, argc, argv, 0, 0, &args);
  char* strNum = args.args[0];
  int baseFrom = baseFromString(args.args[1]);
  int baseTo = baseFromString(args.args[2]);
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
  int inputNum = getStrNumVal(strNum, baseFrom);
  struct charArray outputArray = outNumFrominNum(inputNum, baseTo);
  FILE* pFile = NULL;
  if (args.filename) {
    pFile = fopen(args.filename, "a");
    if (!pFile) {
      printf("Error: couldn't open file, writing to stdout");
    }
  }
  output(outputArray.pArray, pFile);
  free(outputArray.pArray);
  if (pFile) {
    fclose(pFile);
  }
  return 0;
}
