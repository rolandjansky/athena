CREATE TABLE smk2mck_table (
    smk2mck_id   NUMBER(10),
    smk2mck_name  VARCHAR2(200),
    smk2mck_SMK NUMBER(10),
    smk2mck_MCK NUMBER(10),
    smk2mck_comment VARCHAR2(2000),
    CONSTRAINT smk2mck_pk  PRIMARY KEY(smk2mck_id)
);
CREATE INDEX smk_ind  ON smk2mck_table(smk2mck_SMK);
CREATE INDEX mck_ind  ON smk2mck_table(smk2mck_MCK);

