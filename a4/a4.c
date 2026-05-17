//------------------------------------------------------------------------------
// a4.c
//
// A small program "Library Manager" 
//
// Group: 084
//
// Author: 012311363
//------------------------------------------------------------------------------
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define RETURN_CODE_INVALID_ARGC 2
#define ERROR_MESSAGE_INVALID_ARGC "Invalid number of parameters given!\n"
#define RETURN_CODE_FILE_FORMAT_ERROR 3
#define ERROR_MESSAGE_FILE_FORMAT_ERROR "Invalid file format!\n" 
#define RETURN_CODE_FILE_ERROR 4
#define ERROR_MESSAGE_FILE_ERROR "Invalid file submitted!\n"
#define RETURN_CODE_OUT_OF_MEMORY 1 
#define ERROR_MESSAGE_OUT_OF_MEMORY "Out of memory!\n"
#define ERROR_MESSAGE_LIBRARY_IS_EMPTY "Library is empty!\n" 
#define ERROR_MESSAGE_UNKNOWN_COMAND "Unknown command!\n" 
#define BUFFER_SIZE 256 
#define BOOK_DETAILS 6

#define PRINT_COMMAND "print"
#define ADD_COMMAND "add" 
#define REMOVE_COMMAND "remove"
#define SORT_COMMAND "sort"
#define SAVE_COMMAND "save"
#define QUIT_COMMAND "quit"

#define SORT_ISBN "ISBN"
#define SORT_TITLE "title"
#define SORT_AUTHOR "author"
#define SORT_YEAR "year"
#define SORT_PAGES "pages"
#define SORT_COLOR "color"
#define SORT_NONE "none"

typedef char* String; 

typedef enum _SortingCode_
{
  TITLE,
  AUTHOR, 
  ISBN, 
  PAGES, 
  YEAR, 
  COLOR, 
} SortingCode;

typedef struct _Book_
{
  int book_index_;
  String title_;
  String author_; 
  String ISBN_number_;
  int page_count_;
  int published_year_; 
  String cover_color_; 
} Book;

typedef struct _Library_
{
  Book **library_books_ptr_;
  int amount_of_books_; 
  String library_owner_; 
  String sorting_criteria_; 
} Library;

bool checkArgumentCount(int argc); 
bool checkFileFormat(char *file_name); 
FILE* openFileToRead(char *file_name); 
bool checkFileStructure(FILE* file_ptr); 
void getAmount(FILE* file_ptr, Library* library);
String allocateStringArray(int length_of_array); 
Book** allocateLibraryBooks(Library* library); 
void freeAllocatedMemoryInLibrary(Library* library);
String saveLibraryInformation(char *current_line); 
bool saveBookInformation(Library* library, String tmp_string, int information_count, int book_index); 
bool coordinateBookInformation(Library* library, char *current_line, int book_index); 
bool fillLibraryArray(FILE* file_ptr, Library* library); 
void printCommandList(void); 
bool executeValidInput(Library* library); 
String getValidInput(void); 
bool checkLibraryForBooks(Library* library); 
void commandPrint(Library* library);
void commandAdd(Library* library); 
Book** reallocateLibraryBooks(Library* library, int new_amount); 
void commandRemove(Library* library);
void moveBooksBeforeRemoving(Library* library, int book_to_remove); 
FILE* openFileToWrite(Library* library); 
void commandSave(Library* library); 
void writeLibraryIntoFile(Library* library, FILE* backup_file_ptr); 
void commandSort(Library* library); 
bool checkInputedSortingCriteria(Library* library, char* sorting_criteria); 
void executeInputedSortingCriteria(Library* library, SortingCode sorting_criteria); 
void sortStrings(Library* library, String first_string, String second_string, int book_index);
void sortIntegers(Library* library, int first_int, int second_int, int book_index);

//------------------------------------------------------------------------------
/// @brief This function handels the whole program.  
/// @param argc is the count of arguments 
/// @param argv is the argument line 
/// @return 0 or the correct errorcode  
int main(int argc, char** argv)
{
  if(!checkArgumentCount(argc))
  {
    return RETURN_CODE_INVALID_ARGC; 
  }

  if(!checkFileFormat(argv[1]))
  {
    return RETURN_CODE_FILE_FORMAT_ERROR; 
  }

  FILE* file_ptr = openFileToRead(argv[1]); 
  if(file_ptr == NULL)
  {
    return RETURN_CODE_FILE_ERROR; 
  }

  if(!checkFileStructure(file_ptr))
  {
    fclose(file_ptr);
    return RETURN_CODE_FILE_ERROR; 
  }

  Library *library; 
  library = calloc(1, sizeof(Library)); 

  getAmount(file_ptr, library); 

  if(allocateLibraryBooks(library) == NULL)
  {
    fclose(file_ptr);
    free(library); 
    library = NULL; 
    return RETURN_CODE_OUT_OF_MEMORY; 
  }

  if(!fillLibraryArray(file_ptr, library))
  {
    fclose(file_ptr);
    freeAllocatedMemoryInLibrary(library);
    free(library); 
    library = NULL; 
    return RETURN_CODE_OUT_OF_MEMORY; 
  }

  printf("\nWelcome to the ISP Library Manager!\n"); 
  int quit_command = false; 

  do 
  {
    printCommandList(); 

    if(!executeValidInput(library))
    {
      printf("Thank you for using ISP Library Manager! See you later!\n\n"); 
      quit_command = true; 
    }

  } while (!quit_command);

  fclose(file_ptr);
  freeAllocatedMemoryInLibrary(library);
  free(library); 
  library = NULL; 
  return 0; 
}

//------------------------------------------------------------------------------
/// @brief This function checks the amount of arguments from the commandline. 
/// @param argc is the count of arguments 
/// @return boolen if it is a valid number or not 
bool checkArgumentCount(int argc)
{
  if(argc != 2)
  {
    printf("%s", ERROR_MESSAGE_INVALID_ARGC); 
    return false; 
  }
  return true; 
} 

//------------------------------------------------------------------------------
/// @brief This function format from the command line. 
/// @param file_name is the given file name from argv[1]
/// @return boolen if it is a valid format or not 
bool checkFileFormat(char *file_name) 
{
  if(strstr(file_name, ".txt") == NULL) 
  {
    printf("%s", ERROR_MESSAGE_FILE_FORMAT_ERROR);
    return false; 
  }
  return true; 
}

//------------------------------------------------------------------------------
/// @brief This function opens the file wih the file name from the command line. 
/// @param file_name is the given file name from argv[1]
/// @return the file pointer or NULL if here is an error 
FILE* openFileToRead(char *file_name)
{
  FILE* file_ptr = fopen(file_name, "r");

  if(file_ptr == NULL)
  {
    printf("%s", ERROR_MESSAGE_FILE_ERROR); 
    return NULL; 
  }
  return file_ptr; 
}

//------------------------------------------------------------------------------
/// @brief This function checks if the magic number is given. 
/// @param file_ptr is the pointer to the open file 
/// @return boolen if it is a valid format or not 
bool checkFileStructure(FILE* file_ptr)
{
  char magic_number[BUFFER_SIZE]; 
  fgets(magic_number, sizeof(magic_number), file_ptr); 

  if(strcmp(magic_number, "LIBRARY\n") == 0)
  {
    return true; 
  }

  printf("%s", ERROR_MESSAGE_FILE_ERROR); 
  return false;
} 

//------------------------------------------------------------------------------
/// @brief This function gets the amount of books provided from the file. 
/// @param file_ptr is the pointer to the open file 
/// @param library is the pointer to the library 
void getAmount(FILE* file_ptr, Library* library)
{
  rewind(file_ptr); 
  char line[BUFFER_SIZE]; 
  int line_index = 0; 

  while(fgets(line, sizeof(line), file_ptr) != NULL)
  {
    line_index++; 
  }

  line_index -= 3; 
  library->amount_of_books_ = line_index; 
}

//------------------------------------------------------------------------------
/// @brief This function allocates the right amount of memory for a string array. 
/// @param length_of_array is the length of the string which should be allocatd 
/// @return NULL or the correct address 
String allocateStringArray(int length_of_array)
{
  String tmp_ptr; 
  tmp_ptr = calloc(length_of_array + 1, sizeof(char)); 

  if(tmp_ptr == NULL)
  {
    printf("%s", ERROR_MESSAGE_OUT_OF_MEMORY);
    return NULL; 
  }
  return tmp_ptr; 
}

//------------------------------------------------------------------------------
/// @brief This function allocates the right amount of memory for the books. 
/// @param library is the pointer to the library 
/// @return NULL or the correct address 
Book** allocateLibraryBooks(Library* library)
{
  Book **tmp_ptr;
  int number_of_books = library->amount_of_books_; 
  tmp_ptr = calloc(number_of_books, sizeof(Book*));  

  if(tmp_ptr == NULL)
  {
    printf("%s", ERROR_MESSAGE_OUT_OF_MEMORY);
    return NULL; 
  }

  library->library_books_ptr_ = tmp_ptr; 

  for(int book_index = 0; book_index < number_of_books; book_index++)
  {
    library->library_books_ptr_[book_index] = calloc(1, sizeof(Book)); 

    if(library->library_books_ptr_[book_index] == NULL)
    {
      freeAllocatedMemoryInLibrary(library); 
      return NULL; 
    }
  }

  return library->library_books_ptr_; 
}

//------------------------------------------------------------------------------
/// @brief This function frees the allocated memory on the heap. 
/// @param library is the pointer array to be freed 
void freeAllocatedMemoryInLibrary(Library* library)
{
  for(int book_index = 0; book_index < library->amount_of_books_; book_index++)
  {
    free(library->library_books_ptr_[book_index]->title_); 
    free(library->library_books_ptr_[book_index]->author_); 
    free(library->library_books_ptr_[book_index]->ISBN_number_); 
    free(library->library_books_ptr_[book_index]->cover_color_); 
    free(library->library_books_ptr_[book_index]); 
  }
  
  free(library->library_owner_);
  free(library->sorting_criteria_);
  free(library->library_books_ptr_); 
  library->library_books_ptr_ = NULL; 
}

//------------------------------------------------------------------------------
/// @brief This function finds the right information of a string and saves it. 
/// @param current_line is the current line with the information in it 
/// @return NULL or the correct address 
String saveLibraryInformation(char *current_line)
{
  String information; 
  String position_ptr = strchr(current_line, ':'); 

  if(position_ptr == NULL)
  {
    printf("%s", ERROR_MESSAGE_FILE_FORMAT_ERROR); 
    return NULL;
  }

  int position_index = (position_ptr - current_line) + 2; 
  int length_of_information = strlen(current_line) - position_index + 1; 
  information = allocateStringArray(length_of_information + 1); 
  
  if(information == NULL)
  {
    return NULL;
  }

  for(int string_index = 0; current_line[position_index] != '\n'; string_index++)
  {
    information[string_index] = current_line[position_index]; 
    position_index++; 
  }

  information[length_of_information] = '\0'; 
  return information; 
}

//------------------------------------------------------------------------------
/// @brief This function saves the information in the correct allocated book detail list.  
/// @param library is the pointer to the library 
/// @param current_line is the current line with the information in it 
/// @param information_count is the current detail count as the formatted order from the file 
/// @param book_index shows the current book 
/// @return boolen if it was saved correctly  
bool saveBookInformation(Library* library, String information, int information_count, int book_index)
{
  int length_of_information = (int)strlen(information); 
  String tmp_string = allocateStringArray(length_of_information); 
  if(tmp_string == NULL)
  {
    return false; 
  }

  switch (information_count) 
  {
    case TITLE: 
      library->library_books_ptr_[book_index]->title_ = tmp_string; 
      strcpy(library->library_books_ptr_[book_index]->title_, information); 
      break;
    case AUTHOR: 
      library->library_books_ptr_[book_index]->author_ = tmp_string; 
      strcpy(library->library_books_ptr_[book_index]->author_, information); 
      break;
    case ISBN: 
      library->library_books_ptr_[book_index]->ISBN_number_ = tmp_string; 
      strcpy(library->library_books_ptr_[book_index]->ISBN_number_, information); 
      break;
    case PAGES: 
      free(tmp_string); 
      library->library_books_ptr_[book_index]->page_count_ = atoi(information); 
      break;
    case YEAR: 
      free(tmp_string); 
      library->library_books_ptr_[book_index]->published_year_= atoi(information); 
      break;
    case COLOR:
      library->library_books_ptr_[book_index]->cover_color_ = tmp_string; 
      strcpy(library->library_books_ptr_[book_index]->cover_color_, information); 
      break;
    default:
      return false;
      break;
  }
  
  return true; 
}

//------------------------------------------------------------------------------
/// @brief This function splits the book details into strings and give them to another function. 
/// @param library is the pointer to the library 
/// @param current_line is the current line with the information in it 
/// @param book_index shows the current book 
/// @return boolen if it was saved correctly  
bool coordinateBookInformation(Library* library, char *current_line, int book_index)
{
  int information_count = 0; 
  int string_index = 0; 
  library->library_books_ptr_[book_index]->book_index_= book_index + 1; 

  while(current_line[string_index] != '\n')
  {
    char tmp_string[BUFFER_SIZE]; 
    int tmp_string_index = 0; 

    for(tmp_string_index = 0; current_line[string_index] != ',' && current_line[string_index] != '\n'; 
    tmp_string_index++)
    {
      tmp_string[tmp_string_index] = current_line[string_index]; 
      string_index++; 
    }
    
    if(current_line[string_index] == ',') 
    {
      string_index += 2; 
    }

    tmp_string[tmp_string_index] = '\0'; 

    if(!saveBookInformation(library, tmp_string, information_count, book_index))
    {
      return false;
    } 
    information_count++; 
  }
  return true; 
}

//------------------------------------------------------------------------------
/// @brief This function fills the allocated array with the correct data. 
/// @param file_ptr is the pointer to the open file 
/// @param library is the pointer to the library 
/// @return boolen if it was saved correctly 
bool fillLibraryArray(FILE* file_ptr, Library* library)
{
  rewind(file_ptr); 
  int number_of_lines = library->amount_of_books_ + 3; 
  int book_index = 0; 

  for(int line_index = 0; line_index < number_of_lines; line_index++)
  {
    char current_line[BUFFER_SIZE]; 
    fgets(current_line, sizeof(current_line), file_ptr); 
    
    switch (line_index) 
    {
      case 0:
        break; 
      case 1:
        library->library_owner_ = saveLibraryInformation(current_line); 
        
        if(library->library_owner_ == NULL)
        {
          return false; 
        }
        break;
      case 2:
        library->sorting_criteria_ = saveLibraryInformation(current_line); 
        
        if(library->sorting_criteria_ == NULL)
        {
          return false; 
        }
        break;
      default:
        if(!coordinateBookInformation(library, current_line, book_index))
        {
          return false;
        } 
        book_index++; 
        break; 
    }
  }
  return true; 
} 

//------------------------------------------------------------------------------
/// @brief This function prints the command list. 
void printCommandList(void)
{
  printf("\n"); 
  printf("Choose a command:\n");
  printf(" %s: %s out books list\n", PRINT_COMMAND, PRINT_COMMAND);
  printf(" %s: %s a book to the list\n", ADD_COMMAND, ADD_COMMAND);
  printf(" %s: %s a book from the list\n", REMOVE_COMMAND, REMOVE_COMMAND);
  printf(" %s: %s books in the list according to the chosen criteria\n", SORT_COMMAND, SORT_COMMAND);
  printf(" %s: %s progress to a file\n", SAVE_COMMAND, SAVE_COMMAND);
  printf(" %s: close the session\n", QUIT_COMMAND);
}

//------------------------------------------------------------------------------
/// @brief This function asks the user for input. 
/// @return NULL or the correct address 
String getValidInput(void) 
{
  printf("\n"); 
  printf(" > "); 
  fflush(stdout);
  char tmp_input[BUFFER_SIZE]; 
  if(fgets(tmp_input, BUFFER_SIZE, stdin) == NULL)
  {
    return NULL; 
  } 

  printf("\n"); 

  if(strchr(tmp_input, '\n') == NULL) 
  {
    int lost_input; 

    while((lost_input = getchar()) != '\n' && lost_input != EOF)
    {
      continue;
    }
  }

  String newline_ptr = strchr(tmp_input, '\n'); 

  if(newline_ptr != NULL)
  {
    int newline_position = newline_ptr - tmp_input; 
    tmp_input[newline_position] = '\0'; 
  }

  String input = calloc(strlen(tmp_input) + 1, sizeof(char));
  
  if(input == NULL)
  {
    return NULL; 
  }

  strcpy(input, tmp_input); 
  return input; 
}

//------------------------------------------------------------------------------
/// @brief This function handels the user input. 
/// @param library is the pointer to the library 
/// @return boolen if it was executed correctly or quitted 
bool executeValidInput(Library* library) 
{
  bool valid_input = false; 
  String input; 

  do
  {
    input = getValidInput(); 

    if(input == NULL)
    {
      return false; 
    }

    if(strcmp(input, PRINT_COMMAND) == 0)
    {
      if(checkLibraryForBooks(library))
      {
        commandPrint(library); 
      }
    }
    else if(strcmp(input, ADD_COMMAND) == 0)
    {
      commandAdd(library); 
    }
    else if(strcmp(input, REMOVE_COMMAND) == 0)
    {
      if(checkLibraryForBooks(library))
      {
        commandRemove(library); 
      }
    }
    else if(strcmp(input, SORT_COMMAND) == 0)
    {
      if(checkLibraryForBooks(library))
      {
        commandSort(library); 
      }
    }
    else if(strcmp(input, SAVE_COMMAND) == 0)
    {
      if(checkLibraryForBooks(library))
      {
        commandSave(library); 
      }
    }
    else if(strcmp(input, QUIT_COMMAND) == 0)
    {
      free(input);
      input = NULL;
      return false; 
    }
    else 
    {
      printf("%s", ERROR_MESSAGE_UNKNOWN_COMAND); 
    }

    free(input);
    input = NULL;
  } while(!valid_input); 
  return true; 
}

//------------------------------------------------------------------------------
/// @brief This function checks if the library is empty. 
/// @param library is the pointer to the library 
/// @return boolen if it is empty or not 
bool checkLibraryForBooks(Library* library)
{
  if(library->amount_of_books_ == 0)
  {
    printf("%s", ERROR_MESSAGE_LIBRARY_IS_EMPTY); 
    return false; 
  }

  return true; 
}

//------------------------------------------------------------------------------
/// @brief This function prints the library. 
/// @param library is the pointer to the library 
void commandPrint(Library* library)
{
  printf("Library of %s", library->library_owner_);
  printf(" has %d books", library->amount_of_books_); 

  if(strcmp(library->sorting_criteria_, SORT_NONE) != 0)
  {
    printf(" sorted by %s", library->sorting_criteria_); 
  }

  printf(":\n"); 

  for(int library_index = 0; library_index < library->amount_of_books_; library_index++)
  {
    printf("%d) ", library->library_books_ptr_[library_index]->book_index_); 
    printf("%s - ", library->library_books_ptr_[library_index]->title_); 
    printf("%s - ", library->library_books_ptr_[library_index]->author_); 
    printf("%s - ", library->library_books_ptr_[library_index]->ISBN_number_); 
    printf("%d - ", library->library_books_ptr_[library_index]->page_count_); 
    printf("%d - ", library->library_books_ptr_[library_index]->published_year_); 
    printf("%s\n", library->library_books_ptr_[library_index]->cover_color_); 
  }
}

//------------------------------------------------------------------------------
/// @brief This function expands the library for a new book. 
/// @param library is the pointer to the library 
void commandAdd(Library* library)
{
  printf("Enter book data (Title, author, ISBN, number of pages, year of publishing, cover color):\n");
  fflush(stdout);
  printf(" > "); 
  fflush(stdout);
  char book_to_add[BUFFER_SIZE]; 
  fgets(book_to_add, BUFFER_SIZE, stdin); 
  printf("\n"); 
  int new_amount_of_books = library->amount_of_books_ + 1; 

  Book** bigger_library_books_ptr = reallocateLibraryBooks(library, new_amount_of_books); 

  if(bigger_library_books_ptr == NULL)
  {
    return; 
  }

  library->library_books_ptr_ = bigger_library_books_ptr; 

  if(!coordinateBookInformation(library, book_to_add, new_amount_of_books - 1))
  {
    return; 
  }

  library->amount_of_books_++; 
  strcpy(library->sorting_criteria_, SORT_NONE); 
  printf("Book added successfully!\n"); 
}

//------------------------------------------------------------------------------
/// @brief This function reallocates the right amount of memory for the books. 
/// @param library is the pointer to the library 
/// @param new_amount is the new amount of books in the library 
/// @return NULL or the correct address 
Book** reallocateLibraryBooks(Library* library, int new_amount)
{
  if(new_amount < library->amount_of_books_)
  {
    free(library->library_books_ptr_[new_amount]->title_);
    free(library->library_books_ptr_[new_amount]->author_);
    free(library->library_books_ptr_[new_amount]->ISBN_number_);
    free(library->library_books_ptr_[new_amount]->cover_color_);
    free(library->library_books_ptr_[new_amount]);
    library->library_books_ptr_[new_amount] = NULL; 
  }

  Book **bigger_library_books_ptr; 

  if(new_amount == 0)
  {
    bigger_library_books_ptr = realloc(library->library_books_ptr_, sizeof(Book*));
    bigger_library_books_ptr[new_amount]= NULL; 
  }
  else
  {
    bigger_library_books_ptr = realloc(library->library_books_ptr_, new_amount * sizeof(Book*));
  }
  
  if(bigger_library_books_ptr == NULL && new_amount > 0)
  {
    printf("%s", ERROR_MESSAGE_OUT_OF_MEMORY);

    return NULL; 
  }

  if(new_amount > library->amount_of_books_)
  {
    Book* new_book_ptr = calloc(1, sizeof(Book)); 

    if(new_book_ptr == NULL)
    {
      return NULL; 
    }

    bigger_library_books_ptr[new_amount - 1] = new_book_ptr; 
  }

  return bigger_library_books_ptr; 
} 

//------------------------------------------------------------------------------
/// @brief This function removes a book from the library. 
/// @param library is the pointer to the library 
void commandRemove(Library* library)
{
  int book_to_remove = 0; 

  do
  {
    commandPrint(library); 
    printf("\n"); 
    printf("Enter the number of the book in the list to remove:\n");
    fflush(stdout); 
    printf(" > "); 
    fflush(stdout);

    if(scanf("%d", &book_to_remove) != 1) 
    {
      printf("Invalid book number! Try again!\n"); 
    }
    
    int lost_input; 
    while((lost_input = getchar()) != '\n')
    {
      continue;
    }

    printf("\n"); 

    if(book_to_remove >= 1 && book_to_remove <= library->amount_of_books_)
    {
      break;
    }
  } while(1); 

  moveBooksBeforeRemoving(library, book_to_remove); 
  int new_amount_of_books = library->amount_of_books_ - 1; 
  Book** bigger_library_books_ptr = reallocateLibraryBooks(library, new_amount_of_books); 

  if(bigger_library_books_ptr == NULL && new_amount_of_books > 0)
  {
    return; 
  }

  library->library_books_ptr_ = bigger_library_books_ptr; 
  library->amount_of_books_--; 
  printf("Book #%d removed successfully.\n", book_to_remove); 
}

//------------------------------------------------------------------------------
/// @brief This function moves the book which should be removed from the library to the end. 
/// @param library is the pointer to the library 
/// @param book_to_remove is the number of the book which should be removed 
void moveBooksBeforeRemoving(Library* library, int book_to_remove)
{
  int book_to_remove_index = book_to_remove - 1; 
  int amount_of_books = library->amount_of_books_; 

  for(int book_index  = 0; book_index < amount_of_books; book_index++)
  {
    if(book_index == book_to_remove_index)
    {
      Book* tmp_book_ptr = library->library_books_ptr_[book_to_remove_index];

      for(int tmp_book_index = book_to_remove_index; tmp_book_index < amount_of_books - 1; tmp_book_index++)
      {
        library->library_books_ptr_[tmp_book_index] = library->library_books_ptr_[tmp_book_index + 1]; 
        library->library_books_ptr_[tmp_book_index]->book_index_--; 
      }

      library->library_books_ptr_[amount_of_books - 1] = tmp_book_ptr; 
      library->library_books_ptr_[amount_of_books - 1]->book_index_ = amount_of_books - 1; 
    }
  }
}

//------------------------------------------------------------------------------
/// @brief This function saves the books from the library to a backup file. 
/// @param library is the pointer to the library 
void commandSave(Library* library)
{
  FILE* backup_file_ptr = openFileToWrite(library);

  if(backup_file_ptr == NULL)
  {
    fclose(backup_file_ptr); 
    return; 
  }

  writeLibraryIntoFile(library, backup_file_ptr); 
  printf("Library successfully saved to library_by_%s.txt.\n", library->sorting_criteria_); 
  fclose(backup_file_ptr); 
}

//------------------------------------------------------------------------------
/// @brief This function opens a new backup file. 
/// @param library is the pointer to the library 
/// @return the file pointer or NULL if here is an error 
FILE* openFileToWrite(Library* library)
{
  char file_name[BUFFER_SIZE] = ""; 
  strcat(file_name, "library_by_"); 
  strcat(file_name, library->sorting_criteria_);
  strcat(file_name, ".txt");
  FILE* file_ptr = fopen(file_name, "w");

  if(file_ptr == NULL)
  {
    printf("%s", ERROR_MESSAGE_FILE_ERROR); 
    return NULL; 
  }
  return file_ptr; 
}

//------------------------------------------------------------------------------
/// @brief This function writes the books from the library into the backup file. 
/// @param library is the pointer to the library 
/// @param backup_file_ptr is the file pointer to backup file 
void writeLibraryIntoFile(Library* library, FILE* backup_file_ptr)
{
  fprintf(backup_file_ptr, "LIBRARY\n");
  fprintf(backup_file_ptr, "Owner: %s\n", library->library_owner_);
  fprintf(backup_file_ptr, "Sorted by: %s\n", library->sorting_criteria_);

  for(int book_index = 0; book_index < library->amount_of_books_; book_index++)
  {
    fprintf(backup_file_ptr, "%s, ", library->library_books_ptr_[book_index]->title_);
    fprintf(backup_file_ptr, "%s, ", library->library_books_ptr_[book_index]->author_);
    fprintf(backup_file_ptr, "%s, ", library->library_books_ptr_[book_index]->ISBN_number_);
    fprintf(backup_file_ptr, "%d, ", library->library_books_ptr_[book_index]->page_count_);
    fprintf(backup_file_ptr, "%d, ", library->library_books_ptr_[book_index]->published_year_);
    fprintf(backup_file_ptr, "%s\n", library->library_books_ptr_[book_index]->cover_color_);
  }
}

//------------------------------------------------------------------------------
/// @brief This function sorts the books in the library in a specific way. 
/// @param library is the pointer to the library 
void commandSort(Library* library)
{
  char sorting_criteria[BUFFER_SIZE]; 

  do
  {
    printf("Enter sorting criteria (ISBN/title/author/year/pages/color):\n");
    fflush(stdout);
    printf(" > "); 
    fflush(stdout);
    fgets(sorting_criteria, BUFFER_SIZE, stdin); 
    printf("\n"); 
    sorting_criteria[strlen(sorting_criteria) - 1] = '\0'; 

    if(checkInputedSortingCriteria(library, sorting_criteria))
    {
      break; 
    }
  } while(1); 

  strcpy(library->sorting_criteria_, sorting_criteria); 
  printf("Library sorted by %s successfully!\n", sorting_criteria); 
}

bool checkInputedSortingCriteria(Library* library, char* sorting_criteria)
{
  if(strcmp(sorting_criteria, SORT_ISBN) == 0)
  {
    executeInputedSortingCriteria(library, ISBN); 
  }
  else if(strcmp(sorting_criteria, SORT_TITLE) == 0)
  {
    executeInputedSortingCriteria(library, TITLE); 
  }
  else if(strcmp(sorting_criteria, SORT_AUTHOR) == 0)
  {
    executeInputedSortingCriteria(library, AUTHOR); 
  }
  else if(strcmp(sorting_criteria, SORT_YEAR) == 0)
  {
    executeInputedSortingCriteria(library, YEAR); 
  }
  else if(strcmp(sorting_criteria, SORT_PAGES) == 0)
  {
    executeInputedSortingCriteria(library, PAGES); 
  }
  else if(strcmp(sorting_criteria, SORT_COLOR) == 0)
  {
    executeInputedSortingCriteria(library, COLOR); 
  }
  else 
  {
    printf("Invalid sorting criteria! Try again!\n\n");
    return false;
  }
  return true; 
}

void executeInputedSortingCriteria(Library* library, SortingCode sorting_criteria)
{
  for(int book_index = 0; book_index < library->amount_of_books_ - 1; book_index++)
  {
    if(sorting_criteria == YEAR)
    {
      sortIntegers(library, library->library_books_ptr_[book_index]->published_year_, 
      library->library_books_ptr_[book_index + 1]->published_year_, book_index);
    }
    else if(sorting_criteria == PAGES)
    {
      sortIntegers(library, library->library_books_ptr_[book_index]->page_count_, 
      library->library_books_ptr_[book_index + 1]->page_count_, book_index);
    }
    else if(sorting_criteria == TITLE)
    {
      sortStrings(library, library->library_books_ptr_[book_index]->title_, 
      library->library_books_ptr_[book_index + 1]->title_, book_index);
    }
    else if(sorting_criteria == AUTHOR)
    {
      sortStrings(library, library->library_books_ptr_[book_index]->author_, 
      library->library_books_ptr_[book_index + 1]->author_, book_index);
    }
    else if(sorting_criteria == ISBN)
    {
      sortStrings(library, library->library_books_ptr_[book_index]->ISBN_number_, 
      library->library_books_ptr_[book_index + 1]->ISBN_number_, book_index);
    }
    else if(sorting_criteria == COLOR)
    {
      sortStrings(library, library->library_books_ptr_[book_index]->cover_color_, 
      library->library_books_ptr_[book_index + 1]->cover_color_, book_index);
    }
  }
}

void sortStrings(Library* library, String first_string, String second_string, int book_index)
{
  int compare_result = strcmp(first_string, second_string);

  if(compare_result < 0)
  {
    return; 
  }
  else if(compare_result > 0)
  {
    Book* tmp_book_ptr = library->library_books_ptr_[book_index]; 
    library->library_books_ptr_[book_index] = library->library_books_ptr_[book_index + 1]; 
    library->library_books_ptr_[book_index]->book_index_++; 
    library->library_books_ptr_[book_index + 1] = tmp_book_ptr; 
    library->library_books_ptr_[book_index + 1]->book_index_--; 
  }
}

void sortIntegers(Library* library, int first_int, int second_int, int book_index)
{
  if(first_int < second_int)
  {
    return; 
  }
  else if(first_int > second_int)
  {
    Book* tmp_book_ptr = library->library_books_ptr_[book_index]; 
    library->library_books_ptr_[book_index] = library->library_books_ptr_[book_index + 1]; 
    library->library_books_ptr_[book_index]->book_index_++; 
    library->library_books_ptr_[book_index + 1] = tmp_book_ptr; 
    library->library_books_ptr_[book_index + 1]->book_index_--; 
  }
}


