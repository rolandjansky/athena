CREATE TABLE mck_table (
    mck_id   	       NUMBER(10),
    mck_default        BOOLEAN,
    mck_creator        VARCHAR2(100),
    mck_creation_date  VARCHAR2(100),
    mck_comment        VARCHAR2(1000),
    CONSTRAINT         mck_pk                PRIMARY KEY(mck_id),
    CONSTRAINT         mck_default_nn        CHECK(mck_default IS NOT NULL) 
);
CREATE TABLE smck_table (
    smck_id            NUMBER(10),
    smck_slice_type    VARCHAR2(10),
    smck_slice_version VARCHAR2(10),
    smck_config        CLOB,
    smck_config_hash   VARCHAR2(200),
    smck_creator       VARCHAR2(100),
    smck_creation_date VARCHAR2(100),
    smck_comment       VARCHAR2(1000),
    CONSTRAINT         smck_pk               PRIMARY KEY(smck_id),
    CONSTRAINT         smck_slice_type_nn    CHECK(smck_slice_type IS NOT NULL),
    CONSTRAINT         smck_slice_version_u  UNIQUE(smck_slice_version),
    CONSTRAINT         smck_slice_version_nn CHECK(smck_slice_version IS NOT NULL),
    CONSTRAINT         smck_config_nn        CHECK(smck_config IS NOT NULL),
    CONSTRAINT         smck_config_hash_nn   CHECK(smck_config_hash IS NOT NULL),
);
CREATE TABLE mck_to_smck_link (
    link_mck           NUMBER(10),
    link_smck          NUMBER(10),
    CONSTRAINT         link_u                UNIQUE(link_mck,link_smck),
    CONSTRAINT         link_mck_nn           CHECK(link_mck IS NOT NULL),
    CONSTRAINT         link_mck_fk           FOREIGN KEY(link_mck) REFERENCES mck_table(mck_id),
    CONSTRAINT         link_smck_nn          CHECK(link_smck IS NOT NULL),
    CONSTRAINT         link_smck_fk          FOREIGN KEY(link_smck) REFERENCES smck_table(smck_id),
);
CREATE INDEX mck_id_index ON mck_table(mck_id);
CREATE INDEX mck_default_index ON mck_table(mck_default);
CREATE INDEX smck_id_index ON smck_table(smck_id);
CREATE INDEX smck_config_hash_index ON smck_table(smck_config_hash);
CREATE INDEX link_mck_index ON mck_to_smck_link(link_mck);
CREATE INDEX link_smck_index ON mck_to_smck_link(link_smck);
