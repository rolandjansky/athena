DROP INDEX mck_default_index;
DROP INDEX link_mck_index;
DROP INDEX link_smck_index;

ALTER TABLE mck_to_smk_link DROP CONSTRAINT smk_link_creator_nn;
ALTER TABLE mck_to_smk_link DROP CONSTRAINT smk_link_smk_nn;
ALTER TABLE mck_to_smk_link DROP CONSTRAINT smk_link_smk_u;
ALTER TABLE mck_to_smk_link DROP CONSTRAINT smk_link_mck_fk;
ALTER TABLE mck_to_smk_link DROP CONSTRAINT smk_link_mck_nn;
ALTER TABLE mck_to_smck_link DROP CONSTRAINT link_smck_fk;
ALTER TABLE mck_to_smck_link DROP CONSTRAINT link_smck_nn;
ALTER TABLE mck_to_smck_link DROP CONSTRAINT link_mck_fk;
ALTER TABLE mck_to_smck_link DROP CONSTRAINT link_mck_nn;
ALTER TABLE mck_to_smck_link DROP CONSTRAINT link_u;
ALTER TABLE smck_table DROP CONSTRAINT smck_creator_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_athena_version_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_default_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_config_hash_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_config_hash_u;
ALTER TABLE smck_table DROP CONSTRAINT smck_config_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_processing_step_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_tool_patch_version_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_tool_patch_version_u;
ALTER TABLE smck_table DROP CONSTRAINT smck_tool_type_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_slice_type_nn;
ALTER TABLE smck_table DROP CONSTRAINT smck_pk;
ALTER TABLE mck_table DROP CONSTRAINT mck_creator_nn;
ALTER TABLE mck_table DROP CONSTRAINT mck_athena_version_nn;
ALTER TABLE mck_table DROP CONSTRAINT mck_default_nn;
ALTER TABLE mck_table DROP CONSTRAINT mck_pk;

DROP SEQUENCE seq_mck_table_id;
DROP SEQUENCE seq_smck_table_id;

DROP TABLE mck_to_smk_link;
DROP TABLE mck_to_smck_link;
DROP TABLE smck_table;
DROP TABLE mck_table;
