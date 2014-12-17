#include "buffer.h"

// Open the source file and initialize the buffer.
Buffer::Buffer(char *filename)
{
  source_file = new ifstream(filename);
	b = new list<char>();
  fill_buf();
}
  
// Close the file and cleanup.
Buffer::~Buffer ()
{
  if(source_file -> is_open())
	{
		source_file -> close();
	}
	if(source_file != NULL)
	{
		delete source_file;
	}
	if(b != NULL)
	{
		delete b;
	}
}
  
// Remove the next character from the buffer and
// return it.
char Buffer::next_char()
{
	if(b -> empty())
	{
		fill_buf();
	}
  char next = b -> front();
	b -> pop_front();
	return next;
}
  
// Put a character back at the front of the buffer.
void Buffer::unread_char (char c)
{
	b -> push_front(c);
}
  
  
/* If you encounter an error from which you can't recover, print an
  error message and call this method.
*/
void Buffer::buffer_fatal_error()
{
	cout << "Buffer fatal Error" << endl;
	exit(0);
}
  
/* Fill the buffer from the file.  The buffer filling code is
   where all the action happens in this class.  This function
   is responsible for filling the buffer until it is full with
   characters from the file.  Comments should be omitted,
   whitespace should be compressed to a single space character,
   and a '$' should be inserted at the end of the program to
   mark the end of the source file.
*/
void Buffer::fill_buf()
{
  char this_char;
	char prev_char;
	int buf_len = 0;
	
	while(buf_len < MAX_BUFFER_SIZE && !source_file -> eof()) 
	{
		prev_char = this_char;
		source_file -> get(this_char);
		if(!source_file -> eof()) 
		{
			if(is_legal_char(this_char))
			{
				if(this_char != COMMENT_MARKER) 
				{
					if(!is_whitespace(this_char)) 
					{					
						b -> push_back(this_char); 
						buf_len++;
					} else if(!is_whitespace(prev_char)) {
						b -> push_back(' '); 
						buf_len++;
					}
				} else {
					while(this_char != '\n') 
					{
						prev_char = this_char; 
						source_file -> get(this_char); 
					}
				}
			}
			else 
			{
				buffer_fatal_error();
			}
		} else {
			b -> push_back(EOF_MARKER);
			buf_len++;
		}
	}
}

// For debugging: dump the contents of the buffer on the screen.
void Buffer::dump_b()
{
	std::list<char>::iterator this_char;
	for(this_char = b -> begin(); this_char != b -> end(); ++this_char)
	{
		cout << *this_char ;
	}
}


