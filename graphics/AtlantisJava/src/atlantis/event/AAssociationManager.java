package atlantis.event;


import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import atlantis.utils.ALogger;

public class AAssociationManager
{
    private static ALogger logger = ALogger.getLogger(AAssociationManager.class);

    private Map<String, AAssociation> associations = new HashMap<String, AAssociation>();

    public AAssociationManager() {}

    public void clear()
    {
        associations = new HashMap<String, AAssociation>();
    }
    
    public void add(AAssociation a)
    {
    	associations.put(a.getName(), a);
    }

    public void remove(String key)
    {
    	associations.remove(key);
    }

    public int[][] get(String a, String b)
    {
        Object o = associations.get(a + b);
        if(o != null)
        {
            return((AAssociation) o).getData();
        }
        else
        {
            o = associations.get(b + a);
            if(o != null)
            {
                //AOutput.logln("AAssociationManager.get()\n" +
                //    "querying non-existing association from-" + a + "-to-" + b +
                //    "\ncalculating inverse association ...") ;
                AAssociation ass = ((AAssociation) o).invert();
                // following test prevents Atlantis from crashing if there
                // is something wrong with the association between clusters
                // and cells
                if(ass != null)
                {
                    add(ass);
                    return ass.getData();
                }
                else
                {
                        String name = "AAssociationManager.get(): ";
                        logger.debug(name + "No association " +
                                      "found between " + a + " and " + b);
                    return null;
                }
            }
        }
        return null;
    } // get() --------------------------------------------------------------


    public AAssociation getAssociation(String a, String b)
    {
        return (AAssociation) associations.get(a + b);
    }

    public void correct()
    {
        Collection<AAssociation> c = associations.values();
        for (AAssociation a : c) {
            a.purgeIDNotInData();
        }
    }
    

    public String[] getKnownAssociations(String a)
    {
        Collection<AAssociation> c = associations.values();
        Set<String> list = new TreeSet<String>();
        for (AAssociation ass : c) {
            if(ass.getFrom().equals(a))
            {
                list.add(ass.getTo());
            }
            else if(ass.getTo().equals(a))
            {
                list.add(ass.getFrom());
            }
        }
        return (String[]) list.toArray(new String[list.size()]);
    }


    public String getAllKnownAssociations()
    {
        Collection<AAssociation> col = associations.values();
        StringBuilder r = new StringBuilder();
        int c = 0;

        for (AAssociation ass : col) {
            c++;
            r.append(c + " association from " + ass.getFrom() + " to " +
                ass.getTo() + " (name: " + ass.getName() + ")\n");
        }

        return r.toString();
    }
}
