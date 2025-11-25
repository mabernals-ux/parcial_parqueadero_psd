-- ===================================
-- TABLAS CATÁLOGO
-- ===================================

CREATE TABLE IF NOT EXISTS tipos_documento (
    id SERIAL PRIMARY KEY,
    nombre VARCHAR(50) UNIQUE NOT NULL
);

-- Tipos de vehículo (carro, moto...)
CREATE TABLE IF NOT EXISTS tipos_vehiculo (
    id SERIAL PRIMARY KEY,
    nombre VARCHAR(20) UNIQUE NOT NULL
);

-- ===================================
-- TABLAS PRINCIPALES
-- ===================================
--usuarios

CREATE TABLE IF NOT EXISTS usuarios (
    id SERIAL PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL,
    tipo_documento_id INT NOT NULL,
    numero_identificacion VARCHAR(20) UNIQUE NOT NULL,
    saldo DOUBLE PRECISION DEFAULT 0.0,
    CONSTRAINT fk_tipo_doc FOREIGN KEY (tipo_documento_id) REFERENCES tipos_documento(id)
);
SELECT * FROM usuarios
ORDER BY ID;

-- Agregar columna para almacenar el UID de la tarjeta RFID
ALTER TABLE usuarios
ADD COLUMN IF NOT EXISTS uid_rfid VARCHAR(20) UNIQUE;

-- Vehículos
CREATE TABLE IF NOT EXISTS vehiculos (
    id SERIAL PRIMARY KEY,
    usuario_id INT NOT NULL,
    placa VARCHAR(20) UNIQUE NOT NULL,
    tipo_vehiculo_id INT NOT NULL,
    CONSTRAINT fk_usuario FOREIGN KEY (usuario_id) REFERENCES usuarios(id) ON DELETE CASCADE,
    CONSTRAINT fk_tipo_vehiculo FOREIGN KEY (tipo_vehiculo_id) REFERENCES tipos_vehiculo(id)
);

-- Espacios de parqueadero
CREATE TABLE IF NOT EXISTS espacios (
    id SERIAL PRIMARY KEY,
    tipo_vehiculo_id INT NOT NULL,
    estado BOOLEAN DEFAULT FALSE,  -- FALSE = libre, TRUE = ocupado
    vehiculo_id INT,               -- opcional: NULL si está libre
    CONSTRAINT fk_tipo_vehiculo_espacio FOREIGN KEY (tipo_vehiculo_id) REFERENCES tipos_vehiculo(id),
    CONSTRAINT fk_vehiculo_espacio FOREIGN KEY (vehiculo_id) REFERENCES vehiculos(id)
);

-- Tarifas
CREATE TABLE IF NOT EXISTS tarifas (
    id SERIAL PRIMARY KEY,
    tipo_vehiculo_id INT NOT NULL,
    tarifa_hora DOUBLE PRECISION NOT NULL,
    CONSTRAINT fk_tipo_vehiculo_tarifa FOREIGN KEY (tipo_vehiculo_id) REFERENCES tipos_vehiculo(id));
	SELECT*FROM tarifas;


-- Registros (historial de parqueo)
CREATE TABLE IF NOT EXISTS registros (
    id SERIAL PRIMARY KEY,
    vehiculo_id INT NOT NULL,
    espacio_id INT NOT NULL,
    hora_ingreso TIMESTAMP NOT NULL,
    hora_salida TIMESTAMP,
    tiempo_duracion DOUBLE PRECISION,
    total_pago DOUBLE PRECISION,
    CONSTRAINT fk_registro_vehiculo FOREIGN KEY (vehiculo_id) REFERENCES vehiculos(id),
    CONSTRAINT fk_registro_espacio FOREIGN KEY (espacio_id) REFERENCES espacios(id)
);

-- Recargas de saldo
CREATE TABLE IF NOT EXISTS recargas (
    id SERIAL PRIMARY KEY,
    usuario_id INT NOT NULL,
    saldo_anterior DOUBLE PRECISION,
    monto_recargado DOUBLE PRECISION NOT NULL,
    saldo_final DOUBLE PRECISION,
    referencia VARCHAR(50),
    fecha_recarga TIMESTAMP DEFAULT NOW(),
    CONSTRAINT fk_recarga_usuario FOREIGN KEY (usuario_id) REFERENCES usuarios(id)
);

-- tipos de documentos 
DELETE FROM tipos_documento;
INSERT INTO tipos_documento (nombre)
VALUES ('CC'), ('TI'), ('NIT'), ('PAS');


-- Tipos de vehículo
TRUNCATE TABLE tipos_vehiculo RESTART IDENTITY CASCADE;
INSERT INTO tipos_vehiculo (nombre) 
VALUES ('carro'), ('moto');
SELECT * FROM tipos_vehiculo;


--Tarifas según el tipo de vehiculo
TRUNCATE TABLE tarifas RESTART IDENTITY CASCADE;
INSERT INTO tarifas (tipo_vehiculo_id, tarifa_hora) VALUES
(1, 200.00),
(2, 100.00);
SELECT*FROM tarifas;

--espacios del parqueadero 
DELETE FROM espacios;
INSERT INTO espacios (id, tipo_vehiculo_id, estado, vehiculo_id)
VALUES
(1, 1, FALSE, NULL),
(2, 1, FALSE, NULL),
(3, 1, FALSE, NULL),
(4, 1, FALSE, NULL),
(5, 1, FALSE, NULL),
(6, 1, FALSE, NULL),
(7, 1, FALSE, NULL),
(8, 2, FALSE, NULL),
(9, 2, FALSE, NULL),
(10, 2,FALSE, NULL);

DROP TABLE IF EXISTS tarjetas_rfid;

CREATE TABLE IF NOT EXISTS tarjetas_rfid (
    id SERIAL PRIMARY KEY,
    uid VARCHAR(20) UNIQUE,              -- Código único de la tarjeta RFID
    numero_identificacion VARCHAR(20) REFERENCES usuarios(numero_identificacion) ON DELETE SET NULL,
    activa BOOLEAN DEFAULT TRUE,         -- TRUE = activa, FALSE = inactiva
    fecha_asignacion TIMESTAMP DEFAULT NOW()
);


-- Tabla valor minimo
DROP TABLE IF EXISTS valor_minimo;
CREATE TABLE valor_minimo (
    id SERIAL PRIMARY KEY,
    valor DOUBLE PRECISION UNIQUE NOT NULL);
	
INSERT INTO valor_minimo (valor)
VALUES (5000)
ON CONFLICT (valor) DO NOTHING;

-- consultas 
SELECT*FROM recargas;
SELECT*FROM tipos_documento
ORDER BY ID;
SELECT * FROM usuarios
ORDER BY ID;
SELECT * FROM espacios
ORDER BY ID;
SELECT * FROM tarifas
SELECT * FROM vehiculos;
ORDER BY ID;
SELECT * FROM registros
ORDER BY ID;
SELECT*FROM tipos_vehiculo
ORDER BY ID;
SELECT * FROM valor_minimo;

--TRUNCATE TABLE recargas, registros, tarifas, espacios, vehiculos, usuarios, tipos_vehiculo, tipos_documento 
-- IDENTITY CASCADE;

--UPDATE usuarios
--SET uid_rfid = '775DD7C6'
--WHERE numero_identificacion = '1234567890';

ALTER TABLE usuarios
ADD COLUMN uid_rfid VARCHAR(20) UNIQUE;

UPDATE usuarios
SET uid_rfid = NULL
WHERE uid_rfid = '775DD7C6';
	