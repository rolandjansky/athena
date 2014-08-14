package atlantis.utils.xml;

/**
 * The superclass of Integer and Float array parsers used e.g. in parseing
 * arrays stored in xml files
 */

public abstract class AArrayParser
{
    protected int count;
    protected int state;
    protected int sign;
    protected boolean finished;

    protected static final char space = ' ';
    protected static final char cariage = '\n';
    protected static final char zero = '0';
    protected static final char nine = '9';
    protected static final char minus = '-';
    protected static final char plus = '+';
    protected static final char dot = '.';
    protected static final char smallExp = 'e';
    protected static final char bigExp = 'E';

    public abstract void parse(char[] ch, int start, int end);

    public abstract Object getArray();

    /**
     * @return Returns the count.
     */
    public int getCount()
    {
        return count;
    }
}
