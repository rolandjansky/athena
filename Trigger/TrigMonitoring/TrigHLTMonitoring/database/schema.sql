CREATE TABLE master_table (
    id   NUMBER(10),
    name  VARCHAR2(200),
    comment VARCHAR2(2000),
    CONSTRAINT     mt_pk   PRIMARY KEY(id)
);
CREATE TABLE type (
    id             NUMBER(10),
    name  VARCHAR2(50),
    CONSTRAINT     type_pk   PRIMARY KEY(id)
);
CREATE TABLE sig_config (
    id             NUMBER(10),
    name  VARCHAR2(50),
    recostep  VARCHAR2(50),
    data        CLOB,
    script      CLOB,
    CONSTRAINT  sc_pk   PRIMARY KEY(id)
);
CREATE TABLE mt2type (
    master_id      NUMBER(10),
    type_id     NUMBER(10),
    CONSTRAINT mt2type_uc UNIQUE (master_id,type_id),
    CONSTRAINT  mt2type_fk_mt     FOREIGN KEY (master_id) REFERENCES master_table(id),
    CONSTRAINT  mt2type_fk_type    FOREIGN KEY (type_id) REFERENCES type(id),
    CONSTRAINT  master_id_NN  CHECK ( master_id IS NOT NULL),
    CONSTRAINT  type_id_NN CHECK ( type_id IS NOT NULL)
);
CREATE TABLE type2sig (
    type_id        NUMBER(10),
    sig_config_id      NUMBER(10),
    CONSTRAINT type2sig_uc UNIQUE (type_id,sig_config_id),
    CONSTRAINT  type2sig_fk_type     FOREIGN KEY (type_id) REFERENCES type(id),
    CONSTRAINT  type2sig_fk_sig    FOREIGN KEY (sig_config_id) REFERENCES sig_config(id),
    CONSTRAINT  type_id_NN  CHECK ( type_id IS NOT NULL),
    CONSTRAINT  sig_config_id_NN CHECK ( sig_config_id IS NOT NULL)
);
CREATE INDEX master_id_ind  ON mt2type(master_id);
CREATE INDEX type_id_ind ON mt2type(type_id);
CREATE INDEX type_id_ind_1  ON type2sig(type_id);
CREATE INDEX sig_config_id_ind ON type2sig(sig_config_id);
CREATE INDEX type_name_ind ON type(name);
