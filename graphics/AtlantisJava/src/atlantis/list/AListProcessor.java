package atlantis.list;


import javax.swing.Action;
import java.util.Collection;


public interface AListProcessor {
  Action[] getActions(Collection nodes);
}
