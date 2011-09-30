/* ****************************************************************************
 * Copyright (C) 2010 Alex Norton                                             *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify it    *
 * under the terms of the BSD 2-Clause License.                               *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRENTY; without even the implied warranty of                 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 **************************************************************************** */

#ifndef LEXER_H_INCLUDE
#define LEXER_H_INCLUDE

#include <fstream>
using std::ifstream;
#include <iostream>
using std::istream;
using std::cout;
using std::endl;
#include <sstream>
using std::istringstream;
#include <string>
using std::string;

#define BUFFER_SIZE 1024

/**
 * lexer class that will keep track of the line number when reading from an
 * object file.
 */
class lexer {
  public:
    lexer(const string& file_name) :
      _fail_bit(false), _line_number(0), line(), istr(file_name.c_str()) { };
    virtual ~lexer() { };

    inline istream& file_stream() { return istr; }
    inline istringstream& line_stream() { return line; }
    inline int& line_number() { return _line_number; }
    inline int line_number() const { return _line_number; }
    inline bool fail() const { return _fail_bit; }
    inline bool eof() const { return istr.eof() && line.eof(); }

    template<typename T>
    lexer& read(T& t);

  protected:

    bool _fail_bit;
    int _line_number;
    istringstream line;
    ifstream istr;
};

/**
 * Allows the use of a lexer is exactly the same manner as a normal input
 * stream.
 *
 * @param t reference to read from the input stream into
 * @return the lexer that was used when calling read()
 */
template<typename T>
lexer& lexer::read(T& t) {
  char buffer[BUFFER_SIZE];
  string next_line;
  if(!fail() && !eof()) {
    // attempt to read in the string
    line >> t;
    // we failed, attempt to reset the line stream for a new line
    if(line.fail()) {
      if(istr.eof()) {
        return *this;
      }

      if(!line.eof()) {
        _fail_bit = true;
        return *this;
      }

      _line_number++;
      do {
        istr.clear();
        istr.getline(buffer, sizeof(buffer)-1);
        next_line += buffer;
      } while(istr.fail() && !istr.eof());

      line.clear();
      line.str(next_line);
      read(t);
    }
  }
  return *this;
}

/**
 * operator that allows the lexer to read anything this an input stream can
 * read. So "lexer >> int" will work even though the operator>>(lexer, int) has
 * not been explicitly implemented.
 *
 * @param lex the lexer to read from
 * @param t reference to read into
 * @return the lexer that was passed as a parameter
 */
template<typename T>
lexer& operator>>(lexer& lex, T& t) {
  return lex.read(t);
}

#endif /* LEXER_H_INCLUDE */
