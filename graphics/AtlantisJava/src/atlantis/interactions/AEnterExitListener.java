package atlantis.interactions;


/**
 * The listener is supported by AInteractionsManager,
 * it generated events when the mouse is entering or exiting a Hot Region.
 */
public interface AEnterExitListener {

  void entered();
  void exited();
}

