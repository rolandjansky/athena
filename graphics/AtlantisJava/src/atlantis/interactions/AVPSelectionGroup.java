package atlantis.interactions;


public class AVPSelectionGroup extends AInteractionGroup {

  private ARectangleVPSelection rectangleVPSelection;

  public AVPSelectionGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);

    rectangleVPSelection=new ARectangleVPSelection();
    addInteraction(rectangleVPSelection);
  }

}
