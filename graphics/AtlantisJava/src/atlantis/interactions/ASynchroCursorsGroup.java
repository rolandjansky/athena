package atlantis.interactions;


public class ASynchroCursorsGroup extends AInteractionGroup {

  private ASynchroCursorsInteraction mmSelection;

  public ASynchroCursorsGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.CANVAS_GROUP, iManager);

    mmSelection=new ASynchroCursorsInteraction();
    addInteraction(mmSelection);
  }

}
