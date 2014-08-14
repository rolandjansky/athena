package atlantis.nge;

import atlantis.output.ALogInterface;
import atlantis.output.AOutput;

/**
 *
 * @author Adam Davison
 */
public class ANPickResultPrinter implements ANPickListener {

    public void pickCompleted(ANPickResult pr) {
        if (pr != null) {
            AOutput.append("Picked: " + pr.getResult() + "\n\n", ALogInterface.PICK);
        }
    }
}
