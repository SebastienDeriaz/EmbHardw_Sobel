-- EmbHardw
-- Sébastien Deriaz
-- 01.12.2021
--
-- Grayscale custom instruction
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity grayscale is
    port (
        -- Avalon custom interface signals
        Clk    : in std_logic;
        reset : in std_logic;
        dataa  : in std_logic_vector(31 downto 0);
        datab  : in std_logic_vector(31 downto 0);
        start  : in std_logic;
        result : out std_logic_vector(31 downto 0);
        done   : out std_logic
    );
end grayscale;

architecture comp of grayscale is
    --signal result_s  : std_logic_vector (31 downto 0);
begin
    -- Avalon interfacing
    process (Clk, reset)
    begin
        if reset = '1' then
            result <= (others => '0');
        elsif rising_edge(Clk) then
            if (start = '1') then
                result <= std_logic_vector(unsigned(dataa) + unsigned(datab));
                done <= '1';
            else
                done <= '0';
            end if;
        end if;
    end process;
end comp;