CREATE TABLE master_table (
    mt_id   NUMBER(10),
    mt_name  VARCHAR2(200),
    mt_comment VARCHAR2(2000),
    CONSTRAINT     mt_pk   PRIMARY KEY(mt_id)
);
CREATE TABLE type_table (
    tt_id             NUMBER(10),
    tt_name  VARCHAR2(50),
    CONSTRAINT     type_pk1   PRIMARY KEY(tt_id)
);
CREATE TABLE sig_config (
    sc_id             NUMBER(10),
    sc_name  VARCHAR2(50),
    recostep  VARCHAR2(50),
--    sc_data  VARCHAR2(500),
--    sc_script  VARCHAR2(500),
    sc_data        CLOB,
    sc_script      CLOB,
    data_hash VARCHAR2(200),
    script_hash VARCHAR2(200),
    CONSTRAINT  sc_pk   PRIMARY KEY(sc_id)
);
CREATE TABLE mt2type (
    master_id      NUMBER(10),
    type_id1     NUMBER(10),
    CONSTRAINT mt2type_uc UNIQUE (master_id,type_id1),
    CONSTRAINT  mt2type_fk_mt     FOREIGN KEY (master_id) REFERENCES master_table(mt_id),
    CONSTRAINT  mt2type_fk_type    FOREIGN KEY (type_id1) REFERENCES type_table(tt_id),
    CONSTRAINT  master_id_NN  CHECK ( master_id IS NOT NULL),
    CONSTRAINT  type_id_NN1 CHECK ( type_id1 IS NOT NULL)
);
CREATE TABLE type2sig (
    type_id2        NUMBER(10),
    sig_config_id      NUMBER(10),
    CONSTRAINT type2sig_uc UNIQUE (type_id2,sig_config_id),
    CONSTRAINT  type2sig_fk_type     FOREIGN KEY (type_id2) REFERENCES type_table(tt_id),
    CONSTRAINT  type2sig_fk_sig    FOREIGN KEY (sig_config_id) REFERENCES sig_config(sc_id),
    CONSTRAINT  type_id_NN  CHECK ( type_id2 IS NOT NULL),
    CONSTRAINT  sig_config_id_NN CHECK ( sig_config_id IS NOT NULL)
);
CREATE INDEX master_id_ind  ON mt2type(master_id);
CREATE INDEX type_id_ind ON mt2type(type_id1);
CREATE INDEX type_id_ind_1  ON type2sig(type_id2);
CREATE INDEX type2sig_id_ind ON type2sig(sig_config_id);
CREATE INDEX type_name_ind ON type_table(tt_name);
--CREATE INDEX sig_config_id_ind1 ON sig_config(sc_id);
CREATE INDEX sig_config_name_ind ON sig_config(sc_name);
CREATE INDEX sig_config_recostep_ind ON sig_config(recostep);
--CREATE INDEX sig_config_data_ind ON sig_config(sc_data);
--CREATE INDEX sig_config_script_ind ON sig_config(sc_script);
CREATE INDEX sig_config_data_hash_ind ON sig_config(data_hash);
CREATE INDEX sig_config_script_hash_ind ON sig_config(script_hash);


