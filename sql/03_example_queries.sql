-- ============================================================
-- Example queries
-- Smart Pharmacy C++ / MySQL prototype
-- ============================================================

USE farmacia_inteligente;

-- ------------------------------------------------------------
-- 1. Storage zones
-- ------------------------------------------------------------
SELECT
    zone_id,
    zone_code,
    name,
    description,
    temp_min,
    temp_max,
    humidity_min,
    humidity_max,
    active
FROM storage_zones
ORDER BY zone_code;

-- ------------------------------------------------------------
-- 2. Sensors with storage zone
-- ------------------------------------------------------------
SELECT
    s.sensor_id,
    s.sensor_code,
    s.sensor_type,
    s.unit,
    z.zone_code,
    z.name AS zone_name,
    s.active
FROM sensors s
LEFT JOIN storage_zones z
    ON s.zone_id = z.zone_id
ORDER BY s.sensor_code;

-- ------------------------------------------------------------
-- 3. Alarm rules
-- ------------------------------------------------------------
SELECT
    id,
    rule_code,
    sensor_code,
    priority,
    alarm_low,
    alarm_high,
    hysteresis_value,
    activation_delay_seconds,
    return_normal_delay_seconds,
    enabled
FROM alarm_rules
ORDER BY rule_code;

-- ------------------------------------------------------------
-- 4. Last simulated readings
-- ------------------------------------------------------------
SELECT
    r.reading_id,
    s.sensor_code,
    s.sensor_type,
    s.unit,
    r.reading_value,
    r.reading_time,
    r.source
FROM sensor_readings r
LEFT JOIN sensors s
    ON r.sensor_id = s.sensor_id
ORDER BY r.reading_id DESC
LIMIT 20;

-- ------------------------------------------------------------
-- 5. Alarm traceability summary
-- ------------------------------------------------------------
SELECT
    ae.alarm_event_id,
    ae.event_state,
    ae.status,
    ar.rule_code,
    ar.sensor_code,
    ar.priority,
    ae.alarm_direction,
    ae.alarm_value,
    ae.last_alarm_value,
    ae.return_normal_value,
    ae.activation_time_utc,
    ae.acknowledged_time_utc,
    ae.acknowledged_by,
    ae.closed_time_utc,
    ae.closed_by,
    sr.reading_id,
    sr.reading_value,
    sr.source
FROM alarm_events ae
LEFT JOIN alarm_rules ar
    ON ae.rule_id = ar.id
LEFT JOIN sensor_readings sr
    ON ae.reading_id = sr.reading_id
ORDER BY ae.alarm_event_id DESC
LIMIT 20;

-- ------------------------------------------------------------
-- 6. Audit trail
-- ------------------------------------------------------------
SELECT
    audit_id,
    table_name,
    record_id,
    action,
    old_value,
    new_value,
    changed_by,
    changed_at
FROM audit_trail
ORDER BY audit_id DESC
LIMIT 20;

-- ------------------------------------------------------------
-- 7. Pending alarm conditions
-- ------------------------------------------------------------
SELECT
    pending_id,
    sensor_code,
    rule_code,
    alarm_direction,
    first_abnormal_at,
    last_abnormal_at,
    last_reading_value,
    pending_source
FROM alarm_pending_conditions
ORDER BY pending_id DESC;