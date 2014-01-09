/* passphrase-maker: create random passphrases

   Kai Mayer (mayerkai@grinnell.edu)
   Chike Abuah (abuahche@grinnell.edu)
   John David Stone (stone@cs.grinnell.edu)
   Department of Computer Science
   Grinnell College

   created May 30, 2012
   last revised June 14, 2012
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* The passphrases that we generate are assembled from words listed in
   several source files.  In these files, each word occurs on a line by
   itself, with no whitespace other than the line terminators.  We
   presuppose that each word consists entirely of lower-case ASCII letters
   and is no more that MAXWORD letters long.
*/

#define MAXWORD 70

/* Most of the component words of the passphrase are chosen from lists that
   are read in at the beginning of the execution of the program.  The
   following global variables point to dynamically allocated arrays of
   strings that hold those lists.  For each such array, there is also a
   global variable that indicates its size.
*/

char **singular_count_noun_array;
char **plural_count_noun_array;
char **mass_noun_array;
char **adjective_array;
char **intransitive_array;
char **transitive_array;
char **tpspi_intransitive_array;
char **tpspi_transitive_array;
char **past_intransitive_array;
char **past_transitive_array;
char **preposition_array;

unsigned int singular_count_noun_count;
unsigned int plural_count_noun_count;
unsigned int mass_noun_count;
unsigned int adjective_count;
unsigned int intransitive_count;
unsigned int transitive_count;
unsigned int tpspi_intransitive_count;
unsigned int tpspi_transitive_count;
unsigned int past_intransitive_count;
unsigned int past_transitive_count;
unsigned int preposition_count;

/* Some components of some passphrases include digits drawn from the
   following string.
*/

char *digits = "23456789";
unsigned int digit_count = 8;

/* The read_word function collects one word from the specified input file
   allocates storage for it, and returns a pointer to that storage.
*/

char *read_word(FILE *source) {
  static char word[MAXWORD + 1];
  char *result;
  unsigned int position = 0;
  int ch;
  while ((ch = getc(source)) != '\n')
    if (position <= MAXWORD)
      word[position++] = ch;
  word[position] = '\0';
  (void) asprintf(&result, "%s", word);
  return result;
}

/* As they are read from a file, words are initially stored in a singly
   linked list.  Components of such a list belong to the struct
   word_list_component type.
*/

typedef struct word_list_component {
  char *word;
  struct word_list_component *next;
} *word_list;

/* The dyna_alloc function collects all of the words from an input file
   with a specified file name, placing a dynamically allocated array
   containing those words at the address specified by its second argument.
   It returns the number of words in that array.
*/

unsigned int dyna_alloc(char *source_file_name, char ***result) {
  unsigned int word_count = 0;
  int ch;
  word_list new_word;
  word_list list = (word_list) NULL;
  word_list trailer;
  FILE *source = fopen(source_file_name, "r");
  if (source == (FILE *) NULL) {
    fprintf(stderr, "passphrase-maker: unable to open file %s\n",
            source_file_name);
    exit(EXIT_FAILURE);
  }
  while ((ch = getc(source)) != EOF) {
    (void) ungetc(ch, source);
    new_word = (word_list) malloc(sizeof(struct word_list_component));
    if (new_word == (word_list) NULL) {
      fputs("passphrase-maker: unable to allocate word-liststorage\n",
            stderr);
      exit(EXIT_FAILURE);
    }
    new_word->word = read_word(source);
    new_word->next = list;
    list = new_word;
    word_count++;
  }
  fclose(source);
  *result = (char **) calloc(word_count, sizeof(char *));
  unsigned int position = word_count - 1;
  while (list != (word_list) NULL) {
    (*result)[position--] = list->word;
    trailer = list;
    list = list->next;
    free(trailer);
  }
  return word_count;
}

/* The dyna_free procedure deallocates the storage occupied by an array
   that dyna_alloc constructs. */

void dyna_free(int size, char **array) {
  int index;
  for (index = 0; index < size; index++) {
    free(array[index]);
  }
  free(array);
}

/* The randomizer procedure returns a random integer in the range from 0 to
   max - 1.
*/

unsigned int randomizer(unsigned int max) {
  return (int) ((rand() * (double) max) / (double) RAND_MAX);
}

/* A singular noun phrase consists of either a mass noun, optionally
   preceded by an adjective, or a determiner followed by a singular count
   noun, with an adjective optionally placed in between.
*/

char *singular_noun_phrase() {
  char *determiner;
  char *result;
  if (randomizer(2) == 0) {
    if (randomizer(4) == 0)
      (void) asprintf(&result,
                      "%s %s",
                      adjective_array[randomizer(adjective_count)],
                      mass_noun_array[randomizer(mass_noun_count)]);
    else
      (void) asprintf(&result,
                      "%s",
                      mass_noun_array[randomizer(mass_noun_count)]);
  }
  else {
    switch (randomizer(12)) {
    case 0:
      (void) asprintf(&determiner, "%s", "the");
      break;
    case 1:
      (void) asprintf(&determiner, "%s", "this");
      break;
    case 2:
      (void) asprintf(&determiner, "%s", "my");
      break;
    case 3:
      (void) asprintf(&determiner, "%s", "your");
      break;
    case 4:
      (void) asprintf(&determiner, "%s", "his");
      break;
    case 5:
      (void) asprintf(&determiner, "%s", "her");
      break;
    case 6:
      (void) asprintf(&determiner, "%s", "its");
      break;
    case 7:
      (void) asprintf(&determiner, "%s", "our");
      break;
    case 8:
      (void) asprintf(&determiner, "%s", "their");
      break;
    case 9:
      (void) asprintf(&determiner, "%s", "that");
      break;
    case 10:
      (void) asprintf(&determiner, "%s", "any");
      break;
    case 11:
      (void) asprintf(&determiner, "%s", "every");
      break;
    }
    if (randomizer(4) == 0)
      (void) asprintf(&result,
                      "%s %s %s",
                      determiner,
                      adjective_array[randomizer(adjective_count)],
                      singular_count_noun_array[randomizer(singular_count_noun_count)]);
    else
      (void) asprintf(&result,
                      "%s %s",
                      determiner,
                      singular_count_noun_array[randomizer(singular_count_noun_count)]);
    free(determiner);
  }
  return result;
}

/* A plural noun phrase consists a plural noun, optionally preceded by a
   determiner and/or an adjective.  Numerals count as plural determiners.
*/

char *plural_noun_phrase() {
  char *determiner;
  char *result;
  if (randomizer(4) == 0) {
    if (randomizer(4) == 0)
      (void) asprintf(&result,
                      "%s %s",
                      adjective_array[randomizer(adjective_count)],
                      plural_count_noun_array[randomizer(plural_count_noun_count)]);
    else
      (void) asprintf(&result,
                      "%s",
                      plural_count_noun_array[randomizer(plural_count_noun_count)]);
  }
  else {
    switch (randomizer(20)) {
    case 0:
      (void) asprintf(&determiner, "%s", "the");
      break;
    case 1:
      (void) asprintf(&determiner, "%s", "these");
      break;
    case 2:
      (void) asprintf(&determiner, "%s", "my");
      break;
    case 3:
      (void) asprintf(&determiner, "%s", "your");
      break;
    case 4:
      (void) asprintf(&determiner, "%s", "his");
      break;
    case 5:
      (void) asprintf(&determiner, "%s", "her");
      break;
    case 6:
      (void) asprintf(&determiner, "%s", "its");
      break;
    case 7:
      (void) asprintf(&determiner, "%s", "our");
      break;
    case 8:
      (void) asprintf(&determiner, "%s", "their");
      break;
    case 9:
      (void) asprintf(&determiner, "%s", "those");
      break;
    case 10:
      (void) asprintf(&determiner, "%s", "all");
      break;
    case 11:
      (void) asprintf(&determiner, "%s", "some");
      break;
    default:
      (void) asprintf(&determiner,
                      "%c%c",
                      digits[randomizer(digit_count)],
                      digits[randomizer(digit_count)]);
      break;
    }
    if (randomizer(4) == 0)
      (void) asprintf(&result,
                      "%s %s %s",
                      determiner,
                      adjective_array[randomizer(adjective_count)],
                      plural_count_noun_array[randomizer(plural_count_noun_count)]);
    else
      (void) asprintf(&result,
                      "%s %s",
                      determiner,
                      plural_count_noun_array[randomizer(plural_count_noun_count)]);
    free(determiner);
  }
  return result;
}

/* A noun phrase is either singular or plural. */

char *noun_phrase() {
  return (randomizer(2) ? singular_noun_phrase() : plural_noun_phrase());
}

/* A prepositional phrase consists of a preposition and a noun phrase. */

char *prepositional_phrase() {
  char *np = noun_phrase();
  char *result;
  (void) asprintf(&result,
                  "%s %s",
                  preposition_array[randomizer(preposition_count)],
                  np);
  free(np);
  return result;
}  

/* An "infinite" verb phrase is either the reference form of an
   intransitive verb or the reference form of a transitive verb followed by
   a noun phrase.
*/

char *infinite_verb_phrase() {
  char *np;
  char *result;
  if (randomizer(2) == 0)
    (void) asprintf(&result,
                    "%s",
                    intransitive_array[randomizer(intransitive_count)]);
  else {
    np = noun_phrase();
    (void) asprintf(&result,
                    "%s %s",
                    transitive_array[randomizer(transitive_count)],
                    np);
    free(np);
  }
  return result;
}                    

/* A singular verb phrase is:

   the third-person singular present indicative form of an intransitive
   verb;

   the third-person singular present indicative form of a transitive verb,
   followed by a noun phrase;

   the past form of an intransitive verb; or

   the past form of a transitive verb, followed by a noun phrase.
*/

char *singular_verb_phrase() {
  char *np;
  char *result;
  switch (randomizer(4)) {
  case 0:
    (void) asprintf(&result,
                    "%s",
                    tpspi_intransitive_array[randomizer(tpspi_intransitive_count)]);
    break;
  case 1:
    np = noun_phrase();
    (void) asprintf(&result,
                    "%s %s",
                    tpspi_transitive_array[randomizer(tpspi_transitive_count)],
                    np);
    free(np);
    break;
  case 2:
    (void) asprintf(&result,
                    "%s",
                    past_intransitive_array[randomizer(past_intransitive_count)]);
    break;
  case 3:
    np = noun_phrase();
    (void) asprintf(&result,
                    "%s %s",
                    past_transitive_array[randomizer(past_transitive_count)],
                    np);
    free(np);
    break;
  }
  return result;
}

/* A plural verb phrase is:

   the reference form of an intransitive verb;

   the reference form of a transitive verb, followed by a noun phrase;

   the past form of an intransitive verb; or

   the past form of a transitive verb, followed by a noun phrase.
*/

char *plural_verb_phrase() {
  char *np;
  char *result;
  switch (randomizer(4)) {
  case 0:
    (void) asprintf(&result,
                    "%s",
                    intransitive_array[randomizer(intransitive_count)]);
    break;
  case 1:
    np = noun_phrase();
    (void) asprintf(&result,
                    "%s %s",
                    transitive_array[randomizer(transitive_count)],
                    np);
    free(np);
    break;
  case 2:
    (void) asprintf(&result,
                    "%s",
                    past_intransitive_array[randomizer(past_intransitive_count)]);
    break;
  case 3:
    np = noun_phrase();
    (void) asprintf(&result,
                    "%s %s",
                    past_transitive_array[randomizer(past_transitive_count)],
                    np);
    free(np);
    break;
  }
  return result;
}

/* An assertion consists of a noun phrase and a verb phrase, agreeing in
   number, terminated by a full stop.
*/

char *assertion() {
  char *np;
  char *vp;
  char *result;
  if (randomizer(2) == 0) {
    np = singular_noun_phrase();
    vp = singular_verb_phrase();
  }
  else {
    np = plural_noun_phrase();
    vp = plural_verb_phrase();
  }
  (void) asprintf(&result, "%s %s.", np, vp);
  free(np);
  free(vp);
  return result;
}

/* A question consists of either a modal auxiliary, a noun phrase, and an
   "infinite" verb phrase, or of one of the words "do", "does", or "did", a
   noun phrase (singular, plural, or either, respectively), and an infinite
   verb phrase.  In either case, a question mark terminates the question.
*/

char *question() {
  char *np;
  char *vp = infinite_verb_phrase();
  char *result;
  switch (randomizer(11)) {
  case 0:
    np = noun_phrase();
    (void) asprintf(&result, "can %s %s?", np, vp);
    break;
  case 1:
    np = noun_phrase();
    (void) asprintf(&result, "could %s %s?", np, vp);
    break;
  case 2:
    np = noun_phrase();
    (void) asprintf(&result, "did %s %s?", np, vp);
    break;
  case 3:
    np = plural_noun_phrase();
    (void) asprintf(&result, "do %s %s?", np, vp);
    break;
  case 4:
    np = singular_noun_phrase();
    (void) asprintf(&result, "does %s %s?", np, vp);
    break;
  case 5:
    np = noun_phrase();
    (void) asprintf(&result, "may %s %s?", np, vp);
    break;
  case 6:
    np = noun_phrase();
    (void) asprintf(&result, "might %s %s?", np, vp);
    break;
  case 7:
    np = noun_phrase();
    (void) asprintf(&result, "must %s %s?", np, vp);
    break;
  case 8:
    np = noun_phrase();
    (void) asprintf(&result, "should %s %s?", np, vp);
    break;
  case 9:
    np = noun_phrase();
    (void) asprintf(&result, "will %s %s?", np, vp);
    break;
  case 10:
    np = noun_phrase();
    (void) asprintf(&result, "would %s %s?", np, vp);
    break;
  }
  free(np);
  free(vp);
  return result;
}

/* A command consists of an infinite verb phrase, terminated by an
   exclamation point.
*/

char *command() {
  char *vp = infinite_verb_phrase();
  char *result;
  (void) asprintf(&result, "%s!", vp);
  free(vp);
  return result;
}

/* A passphrase consists of an assertion, a question, or a command,
   optionally preceded by a prepositional phrase, with its first letter
   capitalized.
*/

char *passphrase() {
  char *pp;
  char *base;
  char *result;
  if (randomizer(6) == 0) {
    pp = prepositional_phrase();
    switch (randomizer(6)) {
    case 0: case 1: case 2:
      base = assertion();
      break;
    case 3: case 4:
      base = question();
      break;
    case 5:
      base = command();
      break;
    }
    (void) asprintf(&result, "%s%c %s", pp, (randomizer(2) ? ':' : ','), base);
    free(pp);
    free(base);
  }
  else {
    switch (randomizer(6)) {
    case 0: case 1: case 2:
      result = assertion();
      break;
    case 3: case 4:
      result = question();
      break;
    case 5:
      result = command();
      break;
    }
  }
  result[0] = toupper(result[0]);
  return result;
}
  
int main(int argc, char* argv[]) {

  unsigned int number_of_passwords;

  /* Read in the word lists. */

  singular_count_noun_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/singular-count-nouns",
               &singular_count_noun_array);
  plural_count_noun_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/plural-count-nouns",
               &plural_count_noun_array);
  mass_noun_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/mass-nouns",
               &mass_noun_array);
  adjective_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/adjectives",
               &adjective_array);
  intransitive_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/intransitives",
               &intransitive_array);
  transitive_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/transitives",
               &transitive_array); 
  tpspi_intransitive_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/tpspi-intransitives",
               &tpspi_intransitive_array);
  tpspi_transitive_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/tpspi-transitives",
               &tpspi_transitive_array);
  past_intransitive_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/past-intransitives",
               &past_intransitive_array);
  past_transitive_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/past-transitives",
               &past_transitive_array);
  preposition_count =
    dyna_alloc("/home/abuahchu/sysadmin/passphrase/passphrase-maker/prepositions",
               &preposition_array);
  
  /* Set up a random number generator. */

  FILE *urandom = fopen("/dev/urandom", "r");
  unsigned int seed;

  fread(&seed, sizeof(seed), 1, urandom);
  fclose(urandom);
  srand(seed);
  
  /* Examine the command line to determine how many passwords to
     generate.
  */
  if (argc <= 1)
    number_of_passwords = 1;
  else {
    int request = (int) strtol(argv[1], (char **) NULL, 10);
    number_of_passwords = (0 < request) ? request : 1;
  }
  
  /* Generate the passphrases. */

  int counter = 0;
  char *pass;
  int pass_length;
  while (counter++ < number_of_passwords) {
    do {
      pass = passphrase();
      pass_length = strlen(pass);
    } while (pass_length < 18);
    (void) printf("%s\n", pass);
    free(pass);
  }

  /* Free the space for the word arrays. */

  dyna_free(singular_count_noun_count, singular_count_noun_array);
  dyna_free(plural_count_noun_count, plural_count_noun_array);
  dyna_free(mass_noun_count, mass_noun_array);
  dyna_free(adjective_count, adjective_array);
  dyna_free(intransitive_count, intransitive_array);
  dyna_free(transitive_count, transitive_array);
  dyna_free(tpspi_intransitive_count, tpspi_intransitive_array);
  dyna_free(tpspi_transitive_count, tpspi_transitive_array);
  dyna_free(past_intransitive_count, past_intransitive_array);
  dyna_free(past_transitive_count, past_transitive_array);
  dyna_free(preposition_count, preposition_array);

  return EXIT_SUCCESS;
}
