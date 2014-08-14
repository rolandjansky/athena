package atlantis.utils.xml;

/**
 * The parser used to generate arrays of integer values.
 */

public class AIntArrayParser extends AArrayParser
{
    private final static int SKIPPING_BLANKS = 0;
    private final static int CHECKING_MINUS = 1;
    private final static int READING_INT = 2;
    private final static int FINISHING = 3;
    private final static int IGNORE = 4;

    private int[] array;
    private long number = 0;

    public AIntArrayParser(int size)
    {
        array = new int[size];
        count = 0;
        if(size == 0)
            state = IGNORE;
        else
            state = SKIPPING_BLANKS;
        sign = 1;
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
                    case READING_INT:
                        if(c >= zero && c <= nine)
                        {
                            number = number * 10 + c - zero;
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
                    case CHECKING_MINUS:
                        if(c == minus)
                        {
                            sign = -1;
                            state++;
                            continue nextChar;
                        }
                        else
                        {
                            state++;
                            continue thisChar;
                        }
                    case FINISHING:
                        if(c == space || c == cariage)
                        {
                            try
                            {
                                // This is a hack to make 64-bit identifiers work without 
                                // using long internally. It works because for most 
                                // identifiers only the most significant 32 bits are 
                                // actually used. Proper support for 64-bit integer 
                                // identifiers needs to be implemented asap (#618).
                                // -- EJ 09/2012
                                if (number > Integer.MAX_VALUE && number >> 32 != 0) {
                                    number = number >> 32;
                                }
                                array[count] = (int)number * sign;
                            }
                            catch(IndexOutOfBoundsException e)
                            {
                                // the real number of data is more than the 
                                // expected number of data
                                throw new IndexOutOfBoundsException("more");
                            }
                            number = 0;
                            count++;
                            sign = 1;
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
        if(!finished) {
            if (number > Integer.MAX_VALUE) {
                number = number >> 32;
            }
            array[count] = (int)number * sign;
        }
        return array;
    }

}
