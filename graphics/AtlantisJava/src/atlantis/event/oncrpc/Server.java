package atlantis.event.oncrpc;

/**
 * A collection of constants used by the "ONCRPCServer" ONC/RPC program.
 * These constants need to match the respective definitons on the C++ server
 * side, where they are defined in JiveXML/JiveXML/ONCRPCServer.h
 * @author Sebastian Boeser
 */
public interface Server {

  /**
   * The ID by which the server is identified
   */
  public static final int ONCRPCSERVERPROG = 200048965;

  /**
   * The version of the current server interface
   */
  public static final int ONCRPCSERVERVERS = 2;

  /**
   * The ID of the void procdure
   */
  public static final int ONCRPC_NULL_PROC = 0;

  /**
   * The ID of the getStatus procedure
   */
  public static final int ONCRPC_GETSTATUS_PROC = 1;

  /**
   * The ID of the getStreams procedure
   */
  public static final int ONCRPC_GETSTREAMS_PROC = 2;


  /**
   * The ID of the getEvent procedure
   */
  public static final int ONCRPC_GETEVENT_PROC = 3;

  /**
   * The ID of the SetNewEvent procedure - is not implemented in this client
   */
  //public static final int ONCRPC_SETEVENT_PROC = 4;
}

