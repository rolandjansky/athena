CREATE TABLE activeMCK_table (
    activeMCK_id   NUMBER(10),
    activeMCK_name  VARCHAR2(200),
    activeMCK NUMBER(30),
    CONSTRAINT activeMCK_pk  PRIMARY KEY(activeMCK_id)
);
CREATE INDEX activeMCK_ind  ON activeMCK_table(activeMCK);

