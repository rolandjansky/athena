package atlantis.utils.xml;

/**
 * The parser used to generate arrays of float values.
 */

public class AFloatArrayParser extends AArrayParser
{
    private final static int SKIPPING_BLANKS = 0;
    private final static int CHECKING_MINUS = 1;
    private final static int CHECKING_INF1 = 2;
    private final static int CHECKING_INF2 = 3;
    private final static int CHECKING_INF3 = 4;
    private final static int READING_INT = 5;
    private final static int CHECKING_DOT = 6;
    private final static int READING_FLOAT = 7;
    private final static int CHECKING_EXP = 8;
    private final static int CHECKING_EXP_SIGN = 9;
    private final static int READING_EXP = 10;
    private final static int FINISHING = 11;
    private final static int IGNORE = 12;

    private static double[] powers = new double[200 * 2];
    private float[] array;
    private float number = 0;
    private int floatDigits = 0;
    private int exp = 0;
    private int expSign = 1;
    private boolean isInf = false;

    static
    {
        for(int i = 0; i < 2 * 200; i++)
            powers[i] = Math.pow(10, i - 200);
    }

    public AFloatArrayParser(int size)
    {
        array = new float[size];
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

                    case CHECKING_INF1:
                        if(c == 'i' || c == 'I')
                        {
                            state++;
                            continue nextChar;
                        }
                        else
                        {
                            state = READING_INT;
                            continue thisChar;
                        }

                    case CHECKING_INF2:
                        if(c == 'n' || c == 'N')
                        {
                            state++;
                            continue nextChar;
                        }
                        else
                        {
                            state = FINISHING;
                            continue thisChar;
                        }

                    case CHECKING_INF3:
                        if(c == 'f' || c == 'F')
                        {
                            state = FINISHING;
                            isInf = true;
                            continue nextChar;
                        }
                        else
                        {
                            state = FINISHING;
                            continue thisChar;
                        }

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

                    case CHECKING_DOT:
                        if(c == dot)
                        {
                            state++;
                            continue nextChar;
                        }
                        else
                        {
                            state++;
                            continue thisChar;
                        }

                    case READING_FLOAT:
                        if(c >= zero && c <= nine)
                        {
                            number = number * 10 + c - zero;
                            floatDigits++;
                            continue nextChar;
                        }
                        else
                        {
                            state++;
                            continue thisChar;
                        }

                    case CHECKING_EXP:
                        if(c == smallExp || c == bigExp)
                        {
                            state++;
                            continue nextChar;
                        }
                        else
                        {
                            state = FINISHING;
                            continue thisChar;
                        }

                    case CHECKING_EXP_SIGN:
                        if(c == minus)
                        {
                            expSign = -1;
                            state++;
                            continue nextChar;
                        }
                        if(c == plus)
                        {
                            expSign = +1;
                            state++;
                            continue nextChar;
                        }
                        else
                        {
                            state++;
                            continue thisChar;
                        }

                    case READING_EXP:
                        if(c >= zero && c <= nine)
                        {
                            exp = exp * 10 + c - zero;
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
                                if(isInf)
                                {
                                    if(sign == 1)
                                        array[count] = Float.POSITIVE_INFINITY;
                                    else
                                        array[count] = Float.NEGATIVE_INFINITY;
                                    isInf = false;
                                }
                                else
                                    array[count] = (float) (number * sign * powers[expSign * exp - floatDigits + 200]);
                            }
                            catch(IndexOutOfBoundsException e)
                            {
                                // the real number of data is more than the 
                                // expected number of data
                                throw new IndexOutOfBoundsException("more");
                            }
                            state = SKIPPING_BLANKS;
                            sign = 1;
                            exp = 0;
                            expSign = 1;
                            floatDigits = 0;
                            number = 0;
                            count++;
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
        if(!finished)
            array[count] = (float) (number * sign * powers[expSign * exp
                    - floatDigits + 200]);
        return array;
    }

}
