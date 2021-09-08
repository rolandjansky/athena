/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "AsgTools/TProperty.h"

#include <cassert>

namespace asg
{
  namespace detail
  {
    namespace
    {
      /// \brief the type of the \ref Token
      enum class TokenType
      {
        STRING,

        SPACE,

        NUMBER,

        COMMA,

        LIST_OPEN,

        LIST_CLOSE,

        MAP_SEPARATOR,

        MAP_OPEN,

        MAP_CLOSE,

        SUBGROUP
      };

      /// \brief the information in one lexical token in an input string
      struct Token
      {
        /// \brief the type
        TokenType m_type;

        /// \brief the raw input string
        std::string_view m_raw;

        /// \brief the cooked input string
        std::string m_cooked;

        /// \brief the sub-tokens (if this is a sub group)
        std::vector<Token> m_subtokens;
      };

      StatusCode make_string_token (const std::string& input,
                                    std::string::size_type& pos,
                                    Token& token)
      {
        using namespace asg::msgProperty;

        token.m_type = TokenType::STRING;

        char quote = 0;
        if (input[pos] != '\'' && input[pos] != '"')
        {
          ANA_MSG_ERROR ("didn't find quote at position " << pos << " of string: " << input);
          return StatusCode::FAILURE;
        }
        quote = input[pos];
        ++ pos;
        for (; pos < input.size() && input[pos] != quote; ++ pos)
        {
          if (input[pos] == '\\')
          {
            ++ pos;
            if (pos == input.size())
            {
              ANA_MSG_ERROR ("premature end of string: " << input);
              return StatusCode::FAILURE;
            }
            if (input[pos] == '\'' || input[pos] == '"' ||
                input[pos] == '\\' || input[pos] == ',')
            {
              token.m_cooked += input[pos];
            } else
            {
              ANA_MSG_ERROR ("invalid character '" << input[pos] << "' at position " << pos << " of string: " << input);
              return StatusCode::FAILURE;
            }
          } else
          {
            token.m_cooked += input[pos];
          }
        }

        // end-of-string quote handling
        if (pos == input.size())
        {
          ANA_MSG_ERROR ("premature end of string: " << input);
          return StatusCode::FAILURE;
        }
        ++ pos;
        return StatusCode::SUCCESS;
      }

      StatusCode make_number_token (const std::string& input,
                                   std::string::size_type& pos,
                                   Token& token)
      {
        token.m_type = TokenType::NUMBER;

        for (; pos < input.size() &&
               (isalnum (input[pos]) || input[pos] == '.' ||
                input[pos] == '+' || input[pos] == '-'); ++ pos)
          token.m_cooked = input[pos];
        return StatusCode::SUCCESS;
      }

      StatusCode make_space_token (const std::string& input,
                                   std::string::size_type& pos,
                                   Token& token)
      {
        token.m_type = TokenType::SPACE;

        for (; pos < input.size() && isspace (input[pos]); ++ pos)
          token.m_cooked = input[pos];
        return StatusCode::SUCCESS;
      }

      StatusCode make_char_token (const std::string& input,
                                  std::string::size_type& pos,
                                  Token& token)
      {
        using namespace asg::msgProperty;

        if (input[pos] == '[')
          token.m_type = TokenType::LIST_OPEN;
        else if (input[pos] == ']')
          token.m_type = TokenType::LIST_CLOSE;
        else if (input[pos] == ',')
          token.m_type = TokenType::COMMA;
        else if (input[pos] == '{')
          token.m_type = TokenType::MAP_OPEN;
        else if (input[pos] == '}')
          token.m_type = TokenType::MAP_CLOSE;
        else if (input[pos] == ':')
          token.m_type = TokenType::MAP_SEPARATOR;
        else
        {
          ANA_MSG_ERROR ("unexpected token '" << input[pos] << "' at position" << pos << " of string: " << input);
          return StatusCode::FAILURE;
        }
        ++ pos;
        return StatusCode::SUCCESS;
      }

      StatusCode make_token_list (const std::string& input,
                                  std::vector<Token>& tokens)
      {
        using namespace asg::msgProperty;

        tokens.clear ();
        for (std::string::size_type start = 0, pos = 0;
             pos < input.size(); start = pos)
        {
          Token token;
          if (input[pos] == '\'' || input[pos] == '"')
          {
            ANA_CHECK (make_string_token (input, pos, token));
          } else if (isdigit (input[pos]))
          {
            ANA_CHECK (make_number_token (input, pos, token));
          } else if (isspace (input[pos]))
          {
            ANA_CHECK (make_space_token (input, pos, token));
          } else
          {
            ANA_CHECK (make_char_token (input, pos, token));
          }
          token.m_raw = std::string_view (input).substr (start, pos - start);
          tokens.push_back (std::move (token));
        }
        return StatusCode::SUCCESS;
      }



      StatusCode make_token_tree (std::vector<Token>::iterator& iterator,
                                  std::vector<Token>::iterator end,
                                  std::vector<Token>& result,
                                  bool isSubTree)
      {
        using namespace asg::msgProperty;

        while (iterator != end)
        {
          switch (iterator->m_type)
          {
          case TokenType::SPACE:
            ++ iterator;
            // no-op
            break;
          case TokenType::STRING:
          case TokenType::NUMBER:
          case TokenType::COMMA:
          case TokenType::MAP_SEPARATOR:
          case TokenType::SUBGROUP:
            result.emplace_back (std::move (*iterator));
            ++ iterator;
            break;
          case TokenType::LIST_OPEN:
          case TokenType::MAP_OPEN:
            {
              Token subtoken;
              subtoken.m_type = TokenType::SUBGROUP;
              subtoken.m_subtokens.emplace_back (std::move (*iterator));
              ++ iterator;
              if (make_token_tree (iterator, end, subtoken.m_subtokens, true).isFailure())
                return StatusCode::FAILURE;
              subtoken.m_raw = std::string_view (subtoken.m_subtokens.front().m_raw.data(), subtoken.m_subtokens.back().m_raw.end() - subtoken.m_subtokens.front().m_raw.begin());
              result.emplace_back (std::move (subtoken));
            }
            break;
          case TokenType::LIST_CLOSE:
          case TokenType::MAP_CLOSE:
            if (!isSubTree)
            {
              ANA_MSG_ERROR ("unexpected closing token: " << iterator->m_raw);
              return StatusCode::FAILURE;
            }
            result.emplace_back (std::move (*iterator));
            ++ iterator;
            if ((result.back().m_type == TokenType::LIST_CLOSE &&
                 result.front().m_type != TokenType::LIST_OPEN) ||
                (result.back().m_type == TokenType::MAP_CLOSE &&
                 result.front().m_type != TokenType::MAP_OPEN))
            {
              ANA_MSG_ERROR ("missmatched open and closing token: " << result.front().m_raw << " " << result.back().m_raw);
              return StatusCode::FAILURE;
            }
            return StatusCode::SUCCESS;
          }
        }
        if (isSubTree)
        {
          ANA_MSG_ERROR ("missing closing token for: " << result.front().m_raw);
          return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
      }



      std::string quoteString (const std::string& raw)
      {
        std::string quoted;
	for (char ch : raw)
	{
	  if (ch == ',' || ch == '\\' || ch == '"')
	    quoted += '\\';
	  quoted += ch;
	}
	if (quoted.empty())
	  quoted = "\"\"";
        return quoted;
      }
    }



    StatusCode packStringMap (const std::map<std::string,std::string>& value,
                              std::string& result)
    {
      std::string myresult;
      for (const auto& subvalue : value)
      {
	if (!myresult.empty())
	  myresult += ",";
	myresult += quoteString (subvalue.first) + ":" + quoteString (subvalue.second);
      }
      result = "{" + std::move (myresult) + "}";
      return StatusCode::SUCCESS;
    }



    StatusCode packStringVector (const std::vector<std::string>& value,
				 std::string& result)
    {
      std::string myresult;
      for (const auto& subvalue : value)
      {
	if (!myresult.empty())
	  myresult += ",";
	myresult += quoteString (subvalue);
      }
      result = "[" + std::move (myresult) + "]";
      return StatusCode::SUCCESS;
    }



    StatusCode packStringSingle (const std::string& value,
				 std::string& result)
    {
      if (value.find ("\\") == std::string::npos)
      {
        if (value.find ("\'") == std::string::npos)
        {
          result = "\'" + value + "\'";
          return StatusCode::SUCCESS;
        }
        if (value.find ("\"") == std::string::npos)
        {
          result = "\"" + value + "\"";
          return StatusCode::SUCCESS;
        }
      }
      char quote = '\'';
      if (value.find ("\'") != std::string::npos &&
          value.find ("\"") == std::string::npos)
        quote = '\"';
      result = quote;
      for (char ch : value)
      {
        if (ch == quote || ch == '\\')
          result += '\\';
        result += ch;
      }
      result += quote;
      return StatusCode::SUCCESS;
    }




    StatusCode unpackStringSingle (const std::string& value,
				   std::string& result)
    {
      using namespace asg::msgProperty;

      // special case: we don't have a quote, use string verbatim
      if (value.find ('\'') == std::string::npos &&
          value.find ('"') == std::string::npos)
      {
        result = value;
        return StatusCode::SUCCESS;
      }

      std::vector<Token> tokens;
      ANA_CHECK (make_token_list (value, tokens));
      assert (!tokens.empty());
      if (tokens.front().m_type == TokenType::SPACE)
      {
        tokens.erase (tokens.begin());
        assert (!tokens.empty());
      }
      if (tokens.back().m_type == TokenType::SPACE)
      {
        tokens.erase (tokens.begin() + (tokens.size() - 1));
        assert (!tokens.empty());
      }
      if (tokens[0].m_type != TokenType::STRING)
      {
        ANA_MSG_ERROR ("string property didn't start with a quote: " << value);
        return StatusCode::FAILURE;
      }
      if (tokens.size() > 1)
      {
        ANA_MSG_ERROR ("extra text beyond quoted string: " << value);
        return StatusCode::FAILURE;
      }
      result.swap (tokens[0].m_cooked);
      return StatusCode::SUCCESS;
    }



    StatusCode unpackStringVector (const std::string& value,
				   std::vector<std::string>& result)
    {
      using namespace asg::msgProperty;

      std::vector<std::string> myresult;

      std::vector<Token> tokens;
      ANA_CHECK (make_token_list (value, tokens));

      bool done = false;
      unsigned level = 0;
      std::string subresult;
      for (auto& token : tokens)
      {
        if (token.m_type == TokenType::LIST_OPEN)
          ++ level;
        if (level > 1)
        {
          subresult += token.m_raw;
        } else if (level == 0)
        {
          if (token.m_type != TokenType::SPACE)
          {
            if (done)
            {
              ANA_MSG_ERROR ("vector property does not end with ']': " << value);
              return StatusCode::FAILURE;
            } else
            {
              ANA_MSG_ERROR ("vector property does not begin with '[': " << value);
              return StatusCode::FAILURE;
            }
          }
        } else switch (token.m_type)
        {
        case TokenType::MAP_SEPARATOR:
        case TokenType::MAP_OPEN:
        case TokenType::MAP_CLOSE:
        case TokenType::SUBGROUP:
          // not handling this (for now).  if it comes up I fix it,
          // but for now this would be more of a distraction.
          ANA_MSG_ERROR ("(currently) unsupported token in vector: " << token.m_raw);
          return StatusCode::FAILURE;
        case TokenType::SPACE:
          break;
        case TokenType::STRING:
        case TokenType::NUMBER:
          if (!subresult.empty())
          {
            ANA_MSG_ERROR ("invalid entry in vector: " << subresult << token.m_raw);
            return StatusCode::FAILURE;
          }
          subresult = std::move (token.m_raw);
          break;
        case TokenType::COMMA:
          if (subresult.empty())
          {
            ANA_MSG_ERROR ("comma not preceded by value: " << value);
            return StatusCode::FAILURE;
          }
          myresult.push_back (std::move (subresult));
          subresult.clear ();
          break;
        case TokenType::LIST_OPEN:
          if (done)
          {
            ANA_MSG_ERROR ("vector property does not begin with '[': " << value);
            return StatusCode::FAILURE;
          }
          break;
        case TokenType::LIST_CLOSE:
          if (!subresult.empty())
          {
            myresult.push_back (std::move (subresult));
            subresult.clear ();
          } else
          {
            if (!myresult.empty())
            {
              ANA_MSG_ERROR ("vector property has ',' before ']': " << value);
              return StatusCode::FAILURE;
            }
          }
          done = true;
          break;
        }
        if (token.m_type == TokenType::LIST_CLOSE)
        {
          if (level == 0)
          {
            ANA_MSG_ERROR ("extra ']' in vector: " << value);
            return StatusCode::FAILURE;
          }
          -- level;
        }
      }
      if (!done)
      {
        ANA_MSG_ERROR ("did not find a vector in the string");
        return StatusCode::FAILURE;
      }
      result.swap (myresult);
      return StatusCode::SUCCESS;
    }



    StatusCode unpackStringMap (const std::string& value,
                                std::map<std::string,std::string>& result)
    {
      using namespace asg::msgProperty;

      std::vector<Token> tokenTree;
      {
        std::vector<Token> tokenList;
        ANA_CHECK (make_token_list (value, tokenList));
        auto iterator = tokenList.begin();
        ANA_CHECK (make_token_tree (iterator, tokenList.end(), tokenTree, false));
      }

      if (tokenTree.size() != 1 ||
          tokenTree[0].m_type != TokenType::SUBGROUP ||
          tokenTree[0].m_subtokens[0].m_type != TokenType::MAP_OPEN)
      {
        ANA_MSG_ERROR ("failed to recognize value as map: " << value);
        return StatusCode::FAILURE;
      }

      // note that this loop avoids the first and last token,
      // i.e. "{" and "}"
      for (std::size_t index = 1;
           index < tokenTree[0].m_subtokens.size()-1; ++ index)
      {
        Token& mytoken = tokenTree[0].m_subtokens[index];
        switch (index % 4)
        {
        case 0:
          if (mytoken.m_type != TokenType::COMMA)
          {
            ANA_MSG_ERROR ("unexpected token " << mytoken.m_raw);
            return StatusCode::FAILURE;
          }
          break;
        case 1:
          if (mytoken.m_type == TokenType::COMMA ||
              mytoken.m_type == TokenType::MAP_SEPARATOR)
          {
            ANA_MSG_ERROR ("unexpected token " << mytoken.m_raw);
            return StatusCode::FAILURE;
          }
          break;
        case 2:
          if (mytoken.m_type != TokenType::MAP_SEPARATOR)
          {
            ANA_MSG_ERROR ("unexpected token " << mytoken.m_raw);
            return StatusCode::FAILURE;
          }
          break;
        case 3:
          if (mytoken.m_type == TokenType::COMMA ||
              mytoken.m_type == TokenType::MAP_SEPARATOR)
          {
            ANA_MSG_ERROR ("unexpected token " << mytoken.m_raw);
            return StatusCode::FAILURE;
          }
          Token& keytoken = tokenTree[0].m_subtokens[index-2];
          result.emplace (keytoken.m_raw, mytoken.m_raw);
          break;
        }
      }
      if (tokenTree[0].m_subtokens.size() > 2 &&
          tokenTree[0].m_subtokens.size()%4 != 1)
      {
        ANA_MSG_ERROR ("unexpected token " << tokenTree[0].m_subtokens.back().m_raw);
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//               Implementation of function specialisations
//

template< >
int TProperty< std::string >::setFrom( const Property& rhs ) {
   // Check that we have a valid pointer:
   if( ! this->pointer() ) {
      return 1;
   }
     
   std::string asString;
   if (rhs.getCastString (asString).isFailure())
     return 1;
   return this->setString (asString).isFailure();
}

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//       Implementation of the createProperty specialisations/overloads
//

Property* createProperty( const bool& rval ) {
   return new TProperty< bool >( const_cast< bool& >( rval ), Property::BOOL );
}

Property* createProperty( const int& rval ) {
   return new TProperty< int >( const_cast< int& >( rval ), Property::INT );
}

Property* createProperty( const float& rval ) {
   return new TProperty< float >( const_cast< float& >( rval ),
                                  Property::FLOAT );
}

Property* createProperty( const double& rval ) {
   return new TProperty< double >( const_cast< double& >( rval ),
                                   Property::DOUBLE );
}

Property* createProperty( const std::string& rval ) {
   return new TProperty< std::string >( const_cast< std::string& >( rval ),
                                        Property::STRING );
}

Property* createProperty( const std::vector< int >& rval ) {
   typedef Property::IntVector vecInt_t;
   return new TProperty< vecInt_t >( const_cast< vecInt_t& >( rval ),
                                     Property::INTVECTOR );
}

Property* createProperty( const std::vector< float >& rval ) {
   typedef Property::FloatVector vecFloat_t;
   return new TProperty< vecFloat_t >( const_cast< vecFloat_t& >( rval ),
                                       Property::FLOATVECTOR );
}

Property* createProperty( const std::vector< std::string >& rval ) {
   typedef Property::StringVector vecString_t;
   return new TProperty< vecString_t >( const_cast< vecString_t& >( rval ),
                                        Property::STRINGVECTOR );
}

//
////////////////////////////////////////////////////////////////////////////////
