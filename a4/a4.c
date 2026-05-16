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
#define RETURN_CODE_QUIT 
#define BUFFER_SIZE 256 
#define BOOK_DETAILS 6

typedef char* String; 

typedef enum SortingCode_
{
  TITLE,
  AUTHOR, 
  ISBN, 
  PAGES, 
  YEAR, 
  COLOR, 
  NONE, 
} SortingCode;

typedef struct Book_
{
  int book_id_;
  int library_location_; 
  String title_;
  String author_; 
  String ISBN_number_;
  int page_count_;
  int published_year_; 
  String cover_color_; 
} Book;

typedef struct Library_
{
  Book **library_books_ptr_;
  int amount_of_books_; 
  String library_owner_; 
  String sorting_criteria_; 
} Library;

bool checkArgumentCount(int argc); 
bool checkFileFormat(char *file_name); 
FILE* openFile(char *file_name); 
bool checkFileStructure(FILE* file_ptr); 
void getAmount(FILE* file_ptr, Library* library);
String allocateStringArray(int length_of_array); 
Book** allocateLibraryBooks(Library* library); 
void freeAllocatedMemoryInLibrary(Library* library);
String saveLibraryInformation(char *current_line); 
bool saveBookInformation(Library* library, String tmp_string, int information_count, int book_index); 
bool coordinateBookInformation(Library* library, char *current_line, int book_index); 
bool fillLibraryArray(FILE* file_ptr, Library* library); 


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

  FILE* file_ptr = openFile(argv[1]); 
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
    // library->library_books_ptr_ = NULL; 
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

  // welcome 

  printf("%s\n", library->library_owner_);
  printf("%d\n", library->amount_of_books_);
  printf("%s\n", library->sorting_criteria_);
  printf("%d\n", library->library_books_ptr_[0]->book_id_); 
  printf("%d\n", library->library_books_ptr_[0]->library_location_); 
  printf("%s\n", library->library_books_ptr_[0]->title_); 
  printf("%s\n", library->library_books_ptr_[0]->author_); 
  printf("%s\n", library->library_books_ptr_[0]->ISBN_number_); 
  printf("%d\n", library->library_books_ptr_[0]->page_count_); 
  printf("%d\n", library->library_books_ptr_[0]->published_year_); 
  printf("%s\n", library->library_books_ptr_[0]->cover_color_); 


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
FILE* openFile(char *file_name)
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
  library->amount_of_books_ = line_index + 1; 
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
/// @param file_ptr is the pointer to the open file 
/// @param library is the pointer to the library 
/// @return NULL or the correct address 
Book** allocateLibraryBooks(Library* library)
{
  Book **tmp_ptr;
  int number_of_books = library->amount_of_books_; 
  tmp_ptr = calloc(number_of_books, sizeof(char*)); // Books * 

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
/// @param library is the pointer to the library 
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
  information = allocateStringArray(length_of_information); 
  
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
  library->library_books_ptr_[book_index]->book_id_= book_index + 1; 
  library->library_books_ptr_[book_index]->library_location_= book_index; 

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



