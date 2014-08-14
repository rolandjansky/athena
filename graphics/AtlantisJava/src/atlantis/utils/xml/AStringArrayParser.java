package atlantis.utils.xml;

/**
 * The parser used to generate arrays of integer values.
 */

public class AStringArrayParser extends AArrayParser
{
    private final static int SKIPPING_BLANKS = 0;
    private final static int READING_STRING = 1;
    private final static int FINISHING = 2;
    private final static int IGNORE = 3;

    StringBuffer buffer;
    private String[] array;

    public AStringArrayParser(int size)
    {
        array = new String[size];
        buffer = new StringBuffer(20);
        count = 0;
        if(size == 0)
            state = IGNORE;
        else
            state = SKIPPING_BLANKS;
    }

    public void parse(char[] ch, int start, int length)
    {
        if(state == IGNORE)
        {
            finished = true;
            return;
        }

        int end = start + length;
        nextChar: for(int i = start; i < end; i++)
        {
            char c = ch[i];
            thisChar: while(true)
            {

                switch(state)
                {

                    case READING_STRING:
                        if(!(c == space || c == cariage))
                        {
                            buffer.append(c);
                            continue nextChar;
                        }
                        else
                        {
                            state++;
                            continue thisChar;
                        }

                    case SKIPPING_BLANKS:
                        if(c == space || c == cariage)
                            continue nextChar;
                        else
                        {
                            finished = false;
                            state++;
                            continue thisChar;
                        }

                    case FINISHING:
                        if(c == space || c == cariage)
                        {
                            try
                            {
                                array[count++] = new String(buffer);
                            }
                            catch(IndexOutOfBoundsException e)
                            {
                                // the real number of data is more than the 
                                // expected number of data
                                throw new IndexOutOfBoundsException("more");
                            }

                            buffer.setLength(0);
                            state = SKIPPING_BLANKS;
                            finished = true;
                            continue nextChar;
                        }
                        else
                        {
                            throw new NumberFormatException("" + c);
                        }
                }
            } // while true
        } // for
    }

    public Object getArray()
    {
        if(!finished && array.length > 0)
            array[count] = new String(buffer);
        return array;
    }

}
