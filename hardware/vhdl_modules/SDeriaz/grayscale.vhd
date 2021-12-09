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
        reset  : in std_logic;
        dataa  : in std_logic_vector(31 downto 0);
        start  : in std_logic;
        result : out std_logic_vector(31 downto 0);
        done   : out std_logic
    );
end grayscale;

architecture comp of grayscale is
begin
    -- Avalon interfacing
    process (Clk, reset)
        variable R   : unsigned(10 downto 0); -- max 11 bits
        variable G   : unsigned(11 downto 0); -- max 12 bits
        variable B   : unsigned(9 downto 0);  -- max 10 bits
        variable add : unsigned(31 downto 0); -- 32 bits because of result
    begin
        R := (others => '0');
        G := (others => '0');
        B := (others => '0');
        if reset = '1' then
            result <= (others => '0');
        elsif rising_edge(Clk) then
            if (start = '1') then
                R   := resize(unsigned(dataa(15 downto 0) & '0') * 30, 11);
                G   := resize(unsigned(dataa(10 downto 5)) * 59, 12);
                B   := resize(unsigned(dataa(4 downto 0) & '0') * 11, 10);
                add := resize((R + G + B) / 100, 32);
                result <= std_logic_vector(add);

                done   <= '1';
            else
                done <= '0';
            end if;
        end if;
    end process;
end comp;