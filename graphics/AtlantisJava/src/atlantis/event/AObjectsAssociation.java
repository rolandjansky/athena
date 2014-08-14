package atlantis.event;

/**
 * @author Qiang Lu
 *
 */
public class AObjectsAssociation extends AAssociation
{
    private String[] key;
        
    public AObjectsAssociation(String a, String b, String[] associatedKey, 
            int[] associatedIndex, int[] associatedLinkCount, AEvent e)
    {
        super(a, b, associatedLinkCount, associatedIndex,e);
        this.key = new String[associatedKey.length];
        for (int i=0; i<associatedKey.length; i++)
        {
            this.key[i] = associatedKey[i];
        }
    }
    
    public String[] getKey()
    {
        return key;
    }
}
